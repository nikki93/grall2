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
      mOgreName("id" + Ogre::StringConverter::toString(getID()))
{
    //Load the Python script. Events are called by children though.
    NGF::Python::PythonGameObject::setScript(mProperties.getValue("script", 0, ""));

    if (dimensional)
    {
        //Remember which dimensions we're in.
        bool dim1 = Ogre::StringConverter::parseBool(mProperties.getValue("dimension1", 0, "1"));
        bool dim2 = Ogre::StringConverter::parseBool(mProperties.getValue("dimension2", 0, "1"));
        mDimensions = (dim1 ? DimensionManager::DIM_1 : DimensionManager::DIM_NONE) | (dim2 ? DimensionManager::DIM_2 : DimensionManager::DIM_NONE);
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
    delete mBody->getMotionState();
    delete mBody;

    GlbVar.ogreSmgr->destroySceneNode(mNode);
}
//-------------------------------------------------------------------------------
void GraLL2GameObject::unpausedTick(const Ogre::FrameEvent &evt)
{
    //We can't be seen in dimensions we're not in.
    mNode->setVisible(mDimensions & GlbVar.dimMgr->getCurrentDimension(), true);
}
//-------------------------------------------------------------------------------
NGF::MessageReply GraLL2GameObject::receiveMessage(NGF::Message msg)
{
    NGF_NO_REPLY();
}
//-------------------------------------------------------------------------------

//--- Utilities -----------------------------------------------------------------
void GraLL2GameObject::initBody()
{
    GlbVar.phyWorld->addRigidBody(mBody, mDimensions, mDimensions);
    setBulletObject(mBody);
}
//-------------------------------------------------------------------------------
Ogre::Entity *GraLL2GameObject::createBrushEntity()
{
    Ogre::String mesh = mProperties.getValue("brushMeshFile", 0, "Player.mesh");
    Ogre::Entity *ent = GlbVar.ogreSmgr->createEntity(mOgreName, mesh);
    fixBrushMaterials(ent);
    return ent;
}
//-------------------------------------------------------------------------------
void GraLL2GameObject::setDimension(int dimension)
{
    //Save the new dimension info, and reset the btRigidBody flags.
    mDimensions = dimension;
    GlbVar.phyWorld->removeRigidBody(mBody);
    GlbVar.phyWorld->addRigidBody(mBody, mDimensions, mDimensions);
}
//-------------------------------------------------------------------------------

//--- Python interface implementation -------------------------------------------
NGF_PY_BEGIN_IMPL(GraLL2GameObject)
{
    //getPosition
    NGF_PY_METHOD_IMPL(getPosition)
    {
        NGF_PY_RETURN(mNode->getPosition());
    }
    //setPosition
    NGF_PY_METHOD_IMPL(setPosition)
    {
        Ogre::Vector3 vec = py::extract<Ogre::Vector3>(args[0]);

        btTransform oldTrans = mBody->getWorldTransform();
        mBody->setWorldTransform(btTransform(oldTrans.getRotation(), BtOgre::Convert::toBullet(vec)));

        NGF_PY_RETURN();
    }
    //translate
    NGF_PY_METHOD_IMPL(translate)
    {
        mBody->translate(BtOgre::Convert::toBullet(py::extract<Ogre::Vector3>(args[0])));
        NGF_PY_RETURN();
    }
    //getOrientation
    NGF_PY_METHOD_IMPL(getOrientation)
    {
        NGF_PY_RETURN(mNode->getOrientation());
    }
    //setOrientation
    NGF_PY_METHOD_IMPL(setOrientation)
    {
        Ogre::Quaternion rot = py::extract<Ogre::Quaternion>(args[0]);

        btTransform oldTrans = mBody->getWorldTransform();
        mBody->setWorldTransform(btTransform(BtOgre::Convert::toBullet(rot), oldTrans.getOrigin()));

        NGF_PY_RETURN();
    }

    //applyForce
    NGF_PY_METHOD_IMPL(applyForce)
    {
        mBody->applyForce(BtOgre::Convert::toBullet(py::extract<Ogre::Vector3>(args[0])), 
                BtOgre::Convert::toBullet(py::extract<Ogre::Vector3>(args[1])));
        NGF_PY_RETURN();
    }
    //applyCentralForce
    NGF_PY_METHOD_IMPL(applyCentralForce)
    {
        mBody->applyCentralForce(BtOgre::Convert::toBullet(py::extract<Ogre::Vector3>(args[0])));
        NGF_PY_RETURN();
    }
    //getTotalForce
    NGF_PY_METHOD_IMPL(getTotalForce)
    {
        NGF_PY_RETURN(mBody->getTotalForce());
    }
    //applyTorque
    NGF_PY_METHOD_IMPL(applyTorque)
    {
        mBody->applyTorque(BtOgre::Convert::toBullet(py::extract<Ogre::Vector3>(args[0])));
        NGF_PY_RETURN();
    }
    //applyTorqueImpulse
    NGF_PY_METHOD_IMPL(applyTorqueImpulse)
    {
        mBody->applyTorqueImpulse(BtOgre::Convert::toBullet(py::extract<Ogre::Vector3>(args[0])));
        NGF_PY_RETURN();
    }
    //getTotalTorque
    NGF_PY_METHOD_IMPL(getTotalTorque)
    {
        NGF_PY_RETURN(mBody->getTotalTorque());
    }

    //applyImpulse
    NGF_PY_METHOD_IMPL(applyImpulse)
    {
        mBody->applyImpulse(BtOgre::Convert::toBullet(py::extract<Ogre::Vector3>(args[0])), 
                BtOgre::Convert::toBullet(py::extract<Ogre::Vector3>(args[1])));
        NGF_PY_RETURN();
    }
    //applyCentralImpulse
    NGF_PY_METHOD_IMPL(applyCentralImpulse)
    {
        mBody->applyCentralImpulse(BtOgre::Convert::toBullet(py::extract<Ogre::Vector3>(args[0])));
        NGF_PY_RETURN();
    }
    //setLinearVelocity
    NGF_PY_METHOD_IMPL(setLinearVelocity)
    {
        mBody->setLinearVelocity(BtOgre::Convert::toBullet(py::extract<Ogre::Vector3>(args[0])));
        NGF_PY_RETURN();
    }
    //getLinearVelocity
    NGF_PY_METHOD_IMPL(getLinearVelocity)
    {
        NGF_PY_RETURN(mBody->getLinearVelocity());
    }
    //getVelocityInLocalPoint
    NGF_PY_METHOD_IMPL(getVelocityInLocalPoint)
    {
        NGF_PY_RETURN(mBody->getVelocityInLocalPoint(BtOgre::Convert::toBullet(py::extract<Ogre::Vector3>(args[0]))));
    }
    //setAngularVelocity
    NGF_PY_METHOD_IMPL(setAngularVelocity)
    {
        mBody->setAngularVelocity(BtOgre::Convert::toBullet(py::extract<Ogre::Vector3>(args[0])));
        NGF_PY_RETURN();
    }
    //getAngularVelocity
    NGF_PY_METHOD_IMPL(getAngularVelocity)
    {
        NGF_PY_RETURN(mBody->getAngularVelocity());
    }
}
NGF_PY_END_IMPL
//-------------------------------------------------------------------------------
