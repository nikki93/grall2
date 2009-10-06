/*
=========================================
GraLL2GameObject.cpp
=========================================
*/

#define __GRALL2GAMEOBJECT_CPP__

#include "Objects/Main/GraLL2GameObject.h"

//--- NGF events ----------------------------------------------------------------
GraLL2GameObject::GraLL2GameObject(bool dimensional) 
    : NGF::GameObject(Ogre::Vector3(), Ogre::Quaternion(), NGF::ID(), NGF::PropertyList(), Ogre::String()),
      mOgreName("id" + Ogre::StringConverter::toString(getID())),
      mClickedTime(-1)
{
    //Load the Python script. Events are called by children though.
    NGF::Python::PythonGameObject::setScript(mProperties.getValue("script", 0, ""));
    NGF_PY_SAVE_EVENT(alarm);

    if (dimensional)
    {
        Ogre::String dimStr = mProperties.getValue("dimensions", 0, "n");

        if (dimStr != "n")
        {
            mDimensions = Ogre::StringConverter::parseInt(dimStr) & DimensionManager::DIM_BOTH; //Filter out any unnecessary bits.
        }
        else
        {
            bool dim1 = Ogre::StringConverter::parseBool(mProperties.getValue("dimension1", 0, "1"));
            bool dim2 = Ogre::StringConverter::parseBool(mProperties.getValue("dimension2", 0, "1"));
            mDimensions = (dim1 ? DimensionManager::DIM_1 : DimensionManager::NONE) | (dim2 ? DimensionManager::DIM_2 : DimensionManager::NONE);
        }
    }
    else
    {
        mDimensions = DimensionManager::DIM_1 | DimensionManager::DIM_2;
    }
}
//-------------------------------------------------------------------------------
GraLL2GameObject::~GraLL2GameObject()
{
    //We only delete stuff we handle. The child will (hopefully) have done the rest.
    if (mBody)
    {
        delete mBody->getMotionState();
        delete mBody;
    }

    if (mNode)
        GlbVar.ogreSmgr->destroySceneNode(mNode);
}
//-------------------------------------------------------------------------------
void GraLL2GameObject::unpausedTick(const Ogre::FrameEvent &evt)
{
    //Click feedback countdown.
    if (mClickedTime > 0)
    {
        mClickedTime -= evt.timeSinceLastFrame;
    }
    else if (mNode)
    {
        //We can't be seen in dimensions we're not in.
        mNode->setVisible(mDimensions & GlbVar.dimMgr->getCurrentDimension(), true);
    }

    //Tasks.
    updateAlarms(evt.timeSinceLastFrame);
}
//-------------------------------------------------------------------------------
NGF::MessageReply GraLL2GameObject::receiveMessage(NGF::Message msg)
{
    switch (msg.code)
    {
        case MSG_CLICKED:
            if (mNode)
            {
                mNode->setVisible(false);
                mClickedTime = 0.3;
            }
    }
    NGF_NO_REPLY();
}
//-------------------------------------------------------------------------------

//--- Utilities -----------------------------------------------------------------
void GraLL2GameObject::initBody(int specialFlags)
{
    GlbVar.phyWorld->addRigidBody(mBody, mDimensions | specialFlags, mDimensions);
    setBulletObject(mBody);
}
//-------------------------------------------------------------------------------
Ogre::Entity *GraLL2GameObject::createBrushEntity()
{
    Ogre::String mesh = mProperties.getValue("brushMeshFile", 0, "Player.mesh");
    Ogre::Entity *ent = GlbVar.ogreSmgr->createEntity(mOgreName, mesh);
    Util::fixBrushMaterials(ent);
    return ent;
}
//-------------------------------------------------------------------------------
void GraLL2GameObject::setDimension(int dimension)
{
    //Save the new dimension info, and reset the btRigidBody flags.
    mDimensions = dimension;
    short int oldFlags = mBody->getBroadphaseHandle()->m_collisionFilterGroup;
    oldFlags = ((oldFlags >> 2) << 2); //Set rightmost 2 bits to 0 (the dimension bits) so we can start 'from scratch'.
    GlbVar.phyWorld->removeRigidBody(mBody);
    GlbVar.phyWorld->addRigidBody(mBody, mDimensions | oldFlags, mDimensions);
}
//-------------------------------------------------------------------------------

//--- Python interface implementation -------------------------------------------
NGF_PY_BEGIN_IMPL(GraLL2GameObject)
{
    //setAlarm
    GRALL2_PY_ALARM_METHOD(setAlarm);

    //getPosition
    NGF_PY_METHOD_IMPL(getPosition)
    {
        if (!mBody)
            NGF_PY_RETURN();

        NGF_PY_RETURN(mNode->getPosition());
    }
    //setPosition
    NGF_PY_METHOD_IMPL(setPosition)
    {
        if (!mBody)
            NGF_PY_RETURN();

        Ogre::Vector3 vec = py::extract<Ogre::Vector3>(args[0]);

        btTransform oldTrans = mBody->getWorldTransform();
        oldTrans.setOrigin(BtOgre::Convert::toBullet(vec));
        mBody->setWorldTransform(oldTrans);
        mBody->getMotionState()->setWorldTransform(oldTrans);

        NGF_PY_RETURN();
    }
    //translate
    NGF_PY_METHOD_IMPL(translate)
    {
        if (!mBody)
            NGF_PY_RETURN();

        mBody->translate(BtOgre::Convert::toBullet(py::extract<Ogre::Vector3>(args[0])));
        mBody->getMotionState()->setWorldTransform(mBody->getWorldTransform());
        NGF_PY_RETURN();
    }
    //getOrientation
    NGF_PY_METHOD_IMPL(getOrientation)
    {
        if (!mBody)
            NGF_PY_RETURN();

        NGF_PY_RETURN(mNode->getOrientation());
    }
    //setOrientation
    NGF_PY_METHOD_IMPL(setOrientation)
    {
        if (!mBody)
            NGF_PY_RETURN();

        Ogre::Quaternion rot = py::extract<Ogre::Quaternion>(args[0]);

        btTransform oldTrans = mBody->getWorldTransform();
        oldTrans.setRotation(BtOgre::Convert::toBullet(rot));
        mBody->setWorldTransform(oldTrans);
        mBody->getMotionState()->setWorldTransform(oldTrans);

        NGF_PY_RETURN();
    }

    //applyForce
    NGF_PY_METHOD_IMPL(applyForce)
    {
        if (!mBody)
            NGF_PY_RETURN();

        mBody->applyForce(BtOgre::Convert::toBullet(py::extract<Ogre::Vector3>(args[0])), 
                BtOgre::Convert::toBullet(py::extract<Ogre::Vector3>(args[1])));
        NGF_PY_RETURN();
    }
    //applyCentralForce
    NGF_PY_METHOD_IMPL(applyCentralForce)
    {
        if (!mBody)
            NGF_PY_RETURN();

        mBody->applyCentralForce(BtOgre::Convert::toBullet(py::extract<Ogre::Vector3>(args[0])));
        NGF_PY_RETURN();
    }
    //getTotalForce
    NGF_PY_METHOD_IMPL(getTotalForce)
    {
        if (!mBody)
            NGF_PY_RETURN();

        NGF_PY_RETURN(BtOgre::Convert::toOgre(mBody->getTotalForce()));
    }
    //applyTorque
    NGF_PY_METHOD_IMPL(applyTorque)
    {
        if (!mBody)
            NGF_PY_RETURN();

        mBody->applyTorque(BtOgre::Convert::toBullet(py::extract<Ogre::Vector3>(args[0])));
        NGF_PY_RETURN();
    }
    //applyTorqueImpulse
    NGF_PY_METHOD_IMPL(applyTorqueImpulse)
    {
        if (!mBody)
            NGF_PY_RETURN();

        mBody->applyTorqueImpulse(BtOgre::Convert::toBullet(py::extract<Ogre::Vector3>(args[0])));
        NGF_PY_RETURN();
    }
    //getTotalTorque
    NGF_PY_METHOD_IMPL(getTotalTorque)
    {
        if (!mBody)
            NGF_PY_RETURN();

        NGF_PY_RETURN(BtOgre::Convert::toOgre(mBody->getTotalTorque()));
    }

    //applyImpulse
    NGF_PY_METHOD_IMPL(applyImpulse)
    {
        if (!mBody)
            NGF_PY_RETURN();

        mBody->applyImpulse(BtOgre::Convert::toBullet(py::extract<Ogre::Vector3>(args[0])), 
                BtOgre::Convert::toBullet(py::extract<Ogre::Vector3>(args[1])));
        NGF_PY_RETURN();
    }
    //applyCentralImpulse
    NGF_PY_METHOD_IMPL(applyCentralImpulse)
    {
        if (!mBody)
            NGF_PY_RETURN();

        mBody->applyCentralImpulse(BtOgre::Convert::toBullet(py::extract<Ogre::Vector3>(args[0])));
        NGF_PY_RETURN();
    }
    //setLinearVelocity
    NGF_PY_METHOD_IMPL(setLinearVelocity)
    {
        if (!mBody)
            NGF_PY_RETURN();

        mBody->setLinearVelocity(BtOgre::Convert::toBullet(py::extract<Ogre::Vector3>(args[0])));
        NGF_PY_RETURN();
    }
    //getLinearVelocity
    NGF_PY_METHOD_IMPL(getLinearVelocity)
    {
        if (!mBody)
            NGF_PY_RETURN();

        NGF_PY_RETURN(BtOgre::Convert::toOgre(mBody->getLinearVelocity()));
    }
    //getVelocityInLocalPoint
    NGF_PY_METHOD_IMPL(getVelocityInLocalPoint)
    {
        if (!mBody)
            NGF_PY_RETURN();

        btVector3 point = BtOgre::Convert::toBullet(py::extract<Ogre::Vector3>(args[0]));
        NGF_PY_RETURN(BtOgre::Convert::toOgre(mBody->getVelocityInLocalPoint(point)));
    }
    //setAngularVelocity
    NGF_PY_METHOD_IMPL(setAngularVelocity)
    {
        if (!mBody)
            NGF_PY_RETURN();

        mBody->setAngularVelocity(BtOgre::Convert::toBullet(py::extract<Ogre::Vector3>(args[0])));
        NGF_PY_RETURN();
    }
    //getAngularVelocity
    NGF_PY_METHOD_IMPL(getAngularVelocity)
    {
        if (!mBody)
            NGF_PY_RETURN();

        NGF_PY_RETURN(BtOgre::Convert::toOgre(mBody->getAngularVelocity()));
    }
}
NGF_PY_END_IMPL
//-------------------------------------------------------------------------------
