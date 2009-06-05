/*
=========================================
Crate.cpp
=========================================
*/

#define __CRATE_CPP__

#include "Objects/Obstacles/Crate.h"

#define MAX_CRATE_MATERIALS 1

//--- NGF events ----------------------------------------------------------------
Crate::Crate(Ogre::Vector3 pos, Ogre::Quaternion rot, NGF::ID id, NGF::PropertyList properties, Ogre::String name)
    : NGF::GameObject(pos, rot, id , properties, name)
{
    addFlag("Crate");

    //Python init event.
    NGF_PY_CALL_EVENT(init);

    //Read properties.
    Ogre::Real heightDef = Ogre::StringConverter::parseReal(mProperties.getValue("heightDeficiency", 0, "0"));

    if(!(mProperties.getValue("NGF_SERIALISED", 0, "no") == "yes"))
        pos.y -= heightDef * 0.5; //If deficient in height, move down by half the deficiency because midpoint is local origin.

    //Create the Ogre stuff.
    mEntity = GlbVar.ogreSmgr->createEntity(mOgreName, "Crate.mesh");
    mEntity->setMaterialName("Objects/Crate1");
    mNode = GlbVar.ogreSmgr->getRootSceneNode()->createChildSceneNode(mOgreName, pos, rot);
    mNode->attachObject(mEntity);

    //Create the Physics stuff.
    mShape = new btBoxShape(btVector3(0.5,0.75,0.5));
    btScalar mass = 5;
    btVector3 inertia;
    mShape->calculateLocalInertia(mass, inertia);
    BtOgre::RigidBodyState *state = new BtOgre::RigidBodyState(mNode);

    //Height deficiency, for some variety in Crates.
    if (heightDef != 0)
    {
        Ogre::Real actualHeight = 1.5 - heightDef;
        Ogre::Real scale = actualHeight / 1.5;
        mNode->setScale(Ogre::Vector3(1,scale,1));
        mShape->setLocalScaling(btVector3(1,scale,1));
    }

    btGeneric6DofConstraint *constraint = new btGeneric6DofConstraint(*mBody, GlbVar.phyWorld->get, btTransform::getIdentity(), btTransform::getIdentity, false);
    constraint->setLimit(4, 0, 0);
    constraint->setLimit(5, 0, 0);
    constraint->setLimit(6, 0, 0);

    btRigidBody::btRigidBodyConstructionInfo info(mass, state, mShape, inertia);
    info.m_friction = 0.2;
    mBody = new btRigidBody(info);
    initBody();
}
//-------------------------------------------------------------------------------
void Crate::postLoad()
{
    //Python create event.
    NGF_PY_CALL_EVENT(create);
}
//-------------------------------------------------------------------------------
Crate::~Crate()
{
    //Python destroy event.
    NGF_PY_CALL_EVENT(destroy);

    //We only clear up stuff that we did.
    destroyBody();
    delete mShape;

    mNode->detachAllObjects();
    GlbVar.ogreSmgr->destroyEntity(mEntity->getName());
}
//-------------------------------------------------------------------------------
void Crate::unpausedTick(const Ogre::FrameEvent &evt)
{
    GraLL2GameObject::unpausedTick(evt);
    
    //Python utick event.
    NGF_PY_CALL_EVENT(utick, evt.timeSinceLastFrame);
}
//-------------------------------------------------------------------------------
void Crate::pausedTick(const Ogre::FrameEvent &evt)
{
    //Python ptick event.
    NGF_PY_CALL_EVENT(ptick, evt.timeSinceLastFrame);
}
//-------------------------------------------------------------------------------
NGF::MessageReply Crate::receiveMessage(NGF::Message msg)
{
    return GraLL2GameObject::receiveMessage(msg);
}
//-------------------------------------------------------------------------------
void Crate::collide(GameObject *other, btCollisionObject *otherPhysicsObject, btManifoldPoint &contact)
{
    //Python collide event.
    NGF::Python::PythonGameObject *oth = dynamic_cast<NGF::Python::PythonGameObject*>(other);
    if (oth)
        NGF_PY_CALL_EVENT(collide, oth->getConnector());
}
//-------------------------------------------------------------------------------

//--- Python interface implementation -------------------------------------------
/*
NGF_PY_BEGIN_IMPL(Crate)
{
}
NGF_PY_END_IMPL_BASE(GraLL2GameObject)
*/
