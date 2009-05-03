/*
=========================================
Player.cpp
=========================================
*/

#define __PLAYER_CPP__

#include "Objects/Main/Player.h"

//--- NGF events ----------------------------------------------------------------
Player::Player(Ogre::Vector3 pos, Ogre::Quaternion rot, NGF::ID id, NGF::PropertyList properties, Ogre::String name)
    : NGF::GameObject(pos, rot, id , properties, name)
{
    //Creeate the Ogre stuff.
    mEntity = GlbVar.ogreSmgr->createEntity(mOgreName, "Player.mesh");
    mNode = GlbVar.ogreSmgr->getRootSceneNode()->createChildSceneNode(mOgreName, pos, rot);
    mNode->attachObject(mEntity);

    //Create the Physics stuff.
    BtOgre::StaticMeshToShapeConverter converter(mEntity);
    mShape = converter.createSphere();
    btScalar mass = 5;
    btVector3 inertia;
    mShape->calculateLocalInertia(mass, inertia);

    BtOgre::RigidBodyState *state = new BtOgre::RigidBodyState(mNode);
    mBody = new btRigidBody(mass, state, mShape, inertia);
    GlbVar.phyWorld->addRigidBody(mBody, mDimensions, mDimensions);

    //Register our btRigidBody for the NGF collision event.
    setBulletObject(mBody);

    //Player can't be in a dimension that's not being displayed. :P
    setDimension(GlbVar.dimMgr->getCurrentDimension());
}
//-------------------------------------------------------------------------------
Player::~Player()
{
    //We only clear up stuff that we did.
    GlbVar.phyWorld->removeRigidBody(mBody);
    delete mShape;

    mNode->detachObject(mEntity);
    GlbVar.ogreSmgr->destroyEntity(mEntity->getName());
}
//-------------------------------------------------------------------------------
void Player::unpausedTick(const Ogre::FrameEvent &evt)
{
    GraLL2GameObject::unpausedTick(evt);
}
//-------------------------------------------------------------------------------
void Player::pausedTick(const Ogre::FrameEvent &evt)
{
}
//-------------------------------------------------------------------------------
NGF::MessageReply Player::receiveMessage(NGF::Message msg)
{
    switch (msg.code)
    {
        case MSG_KEYPRESSED:
            switch (msg.getParam<OIS::KeyCode>(0))
            {
                case OIS::KC_Z:
                    GlbVar.dimMgr->switchDimension();
                    setDimension(GlbVar.dimMgr->getCurrentDimension());
            }

            NGF_NO_REPLY();
    }

    NGF_NO_REPLY();
}
//-------------------------------------------------------------------------------
void Player::collide(GameObject *other, btCollisionObject *otherPhysicsObject, btManifoldPoint &contact)
{
}
//-------------------------------------------------------------------------------

//--- Python interface implementation -------------------------------------------
/*
NGF_PY_BEGIN_IMPL(Player)
{
}
*/
