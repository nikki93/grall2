/*
=========================================
StaticBrush.cpp
=========================================
*/

#define __STATICBRUSH_CPP__

#include "Objects/Main/StaticBrush.h"

//--- NGF events ----------------------------------------------------------------
StaticBrush::StaticBrush(Ogre::Vector3 pos, Ogre::Quaternion rot, NGF::ID id, NGF::PropertyList properties, Ogre::String name)
    : NGF::GameObject(pos, rot, id , properties, name)
{
    //Create the Ogre stuff.
    Ogre::String mesh = properties.getValue("brushMeshFile", 0, "Player.mesh");
    mEntity = GlbVar.ogreSmgr->createEntity(mOgreName, mesh);
    mNode = GlbVar.ogreSmgr->getRootSceneNode()->createChildSceneNode(mOgreName, pos, rot);
    mNode->attachObject(mEntity);
    mEntity->setMaterialName("Player/TEXFACE/Player.png");

    //Create the Physics stuff.
    BtOgre::StaticMeshToShapeConverter converter(mEntity);
    mShape = converter.createTrimesh();
    //btScalar mass = 5;
    //btVector3 inertia;
    //mShape->calculateLocalInertia(mass, inertia);

    BtOgre::RigidBodyState *state = new BtOgre::RigidBodyState(mNode);
    mBody = new btRigidBody(0, state, mShape, btVector3(0,0,0));
    initBody();
}
//-------------------------------------------------------------------------------
StaticBrush::~StaticBrush()
{
    //We only clear up stuff that we did.
    destroyBody();
    delete mShape;

    mNode->detachObject(mEntity);
    GlbVar.ogreSmgr->destroyEntity(mEntity->getName());
}
//-------------------------------------------------------------------------------
void StaticBrush::unpausedTick(const Ogre::FrameEvent &evt)
{
    GraLL2GameObject::unpausedTick(evt);
}
//-------------------------------------------------------------------------------
void StaticBrush::pausedTick(const Ogre::FrameEvent &evt)
{
}
//-------------------------------------------------------------------------------
NGF::MessageReply StaticBrush::receiveMessage(NGF::Message msg)
{
    NGF_NO_REPLY();
}
//-------------------------------------------------------------------------------
void StaticBrush::collide(GameObject *other, btCollisionObject *otherPhysicsObject, btManifoldPoint &contact)
{
}
//-------------------------------------------------------------------------------

//--- Python interface implementation -------------------------------------------
/*
NGF_PY_BEGIN_IMPL(StaticBrush)
{
}
*/
