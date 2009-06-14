
/*
=========================================
Bomb.cpp
=========================================
*/

#define __BOMB_CPP__

#include "Objects/Obstacles/Bomb.h"

//--- NGF events ----------------------------------------------------------------
Bomb::Bomb(Ogre::Vector3 pos, Ogre::Quaternion rot, NGF::ID id, NGF::PropertyList properties, Ogre::String name)
    : NGF::GameObject(pos, rot, id , properties, name)
{
    addFlag("Bomb");
    addFlag("Explosive");

    //Python init event.
    NGF_PY_CALL_EVENT(init);

    //Create the Ogre stuff.
    mEntity = GlbVar.ogreSmgr->createEntity(mOgreName, "Bomb.mesh");
    mEntity->setMaterialName("Objects/Bomb");
    mNode = GlbVar.ogreSmgr->getRootSceneNode()->createChildSceneNode(mOgreName, pos, rot);
    mNode->attachObject(mEntity);

    //Create the Physics stuff.
    BtOgre::StaticMeshToShapeConverter converter(mEntity);
    mShape = converter.createSphere();

    BtOgre::RigidBodyState *state = new BtOgre::RigidBodyState(mNode);
    mBody = new btRigidBody(0, state, mShape, btVector3(0,0,0));
    GlbVar.phyWorld->addRigidBody(mBody, mDimensions | DimensionManager::NO_DIM_CHECK, mDimensions);
    setBulletObject(mBody);
    mBody->setCollisionFlags(mBody->getCollisionFlags() | btCollisionObject::CF_NO_CONTACT_RESPONSE);
}
//-------------------------------------------------------------------------------
void Bomb::postLoad()
{
    //Python create event.
    NGF_PY_CALL_EVENT(create);
}
//-------------------------------------------------------------------------------
Bomb::~Bomb()
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
void Bomb::unpausedTick(const Ogre::FrameEvent &evt)
{
    //Python utick event.
    NGF_PY_CALL_EVENT(utick, evt.timeSinceLastFrame);
}
//-------------------------------------------------------------------------------
void Bomb::pausedTick(const Ogre::FrameEvent &evt)
{
    //Python ptick event.
    NGF_PY_CALL_EVENT(ptick, evt.timeSinceLastFrame);
}
//-------------------------------------------------------------------------------
NGF::MessageReply Bomb::receiveMessage(NGF::Message msg)
{
    switch (msg.code)
    {
        case MSG_EXPLODE:
            explode();
            NGF_NO_REPLY();
    }
    
    return GraLL2GameObject::receiveMessage(msg);
}
//-------------------------------------------------------------------------------
void Bomb::collide(GameObject *other, btCollisionObject *otherPhysicsObject, btManifoldPoint &contact)
{
    //Python collide event.
    NGF::Python::PythonGameObject *oth = dynamic_cast<NGF::Python::PythonGameObject*>(other);
    if (oth)
        NGF_PY_CALL_EVENT(collide, oth->getConnector());
}
//-------------------------------------------------------------------------------

//--- Non-NGF -------------------------------------------------------------------
void Bomb::explode()
{
    //TODO: Add particle effect for explosion.
    GlbVar.goMgr->requestDestroy(getID());
}
//-------------------------------------------------------------------------------

//--- Python interface implementation -------------------------------------------
/*
NGF_PY_BEGIN_IMPL(Bomb)
{
}
NGF_PY_END_IMPL_BASE(GraLL2GameObject)
*/
