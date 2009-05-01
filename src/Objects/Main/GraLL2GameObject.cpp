/*
=========================================
GraLL2GameObject.cpp
=========================================
*/

#define __GRALL2GAMEOBJECT_CPP__

#include "Objects/Main/GraLL2GameObject.h"

//--- NGF events ----------------------------------------------------------------
GraLL2GameObject::GraLL2GameObject(Ogre::Vector3 pos, Ogre::Quaternion rot, NGF::ID id, NGF::PropertyList properties, Ogre::String name) 
    : NGF::GameObject(pos, rot, id , properties, name),
      mOgreName("id" + StringConverter::toString(mID))
{
    //Load the Python script. Events are called by children though.
    NGF::Python::PythonGameObject::setScript(properties.getValue("script", 0, ""));

    //Remember which dimensions we're in.
    bool dim1 = Ogre::StringConverter::parseBool(properties.getValue("dimension1", 0, "1"));
    bool dim2 = Ogre::StringConverter::parseBool(properties.getValue("dimension2", 0, "1"));
    mDimensions = (dim1 ? DimensionManager::DIM_1 : DimensionManager::DIM_NONE) | (dim2 ? DimensionManager::DIM_2 : DimensionManager::DIM_NONE);

    //--- What a our children might do ------------------------------------------
    //Ogre::String idStr = Ogre::StringConverter::toString(id);

    //Create the Ogre stuff.
    //mEntity = GlbVar.ogreSmgr->createEntity(idStr + "-testEntity", "Player.mesh");
    //mNode = GlbVar.ogreSmgr->getRootSceneNode()->createChildSceneNode(idStr + "-testNode", pos, rot);
    //mNode->attachObject(mEntity);

    //Create the Physics stuff.
    //BtOgre::StaticMeshToShapeConverter converter(mEntity);
    //mShape = converter.createSphere();
    //btScalar mass = 5;
    //btVector3 inertia;
    //mShape->calculateLocalInertia(mass, inertia);

    //BtOgre::RigidBodyState *ninjaState = new BtOgre::RigidBodyState(mNode);
    //mBody = new btRigidBody(mass, ninjaState, mShape, inertia);
    //GlbVar.phyWorld->addRigidBody(mBody, mDimensions, mDimensions);
    //setBulletObject(mBody);
}
//-------------------------------------------------------------------------------
GraLL2GameObject::~GraLL2GameObject()
{
    //--- What our children might do --------------------------------------------
    //delete mShape;
    //GlbVar.phyWorld->removeRigidBody(mBody);
    //mNode->detachObject(mEntity);
    //GlbVar.ogreSmgr->destroyEntity(mEntity->getName());
    
    delete mBody->getMotionState();
    delete mBody;

    GlbVar.ogreSmgr->destroySceneNode(mNode);
}
//-------------------------------------------------------------------------------
void GraLL2GameObject::unpausedTick(const Ogre::FrameEvent &evt)
{
    //We can't be seen in dimensions we're not in.
    mNode->setVisible(mDimensions & GlbVar.dimMgr->getCurrentDimension(), true);

    //--- What the Player might do ----------------------------------------------
    //goToDimension(GlbVar.dimMgr->getCurrentDimension());
}
//-------------------------------------------------------------------------------

//--- Utilities -----------------------------------------------------------------
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
