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
    addFlag("Player");

    //Python init event.
    NGF_PY_CALL_EVENT(init);

    //Create the Ogre stuff.
    mEntity = GlbVar.ogreSmgr->createEntity(mOgreName, "Player.mesh");
    mEntity->setMaterialName("Player");
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
    mBody->setActivationState(DISABLE_DEACTIVATION);
    initBody();

    //Player can't be in a dimension that's not being displayed. :P
    setDimension(GlbVar.dimMgr->getCurrentDimension());

    //Python create event.
    NGF_PY_CALL_EVENT(create);
}
//-------------------------------------------------------------------------------
Player::~Player()
{
    //Python destroy event.
    NGF_PY_CALL_EVENT(destroy);

    //We only clear up stuff that we did.
    destroyBody();
    delete mShape;

    mNode->detachObject(mEntity);
    GlbVar.ogreSmgr->destroyEntity(mEntity->getName());
}
//-------------------------------------------------------------------------------
void Player::unpausedTick(const Ogre::FrameEvent &evt)
{
    GraLL2GameObject::unpausedTick(evt);

    const Ogre::Real force = 100;

    if (isKeyDown(OIS::KC_I))
        mBody->applyCentralForce(btVector3(-force,0,0));
    if (isKeyDown(OIS::KC_K))
        mBody->applyCentralForce(btVector3(force,0,0));
    if (isKeyDown(OIS::KC_J))
        mBody->applyCentralForce(btVector3(0,0,force));
    if (isKeyDown(OIS::KC_L))
        mBody->applyCentralForce(btVector3(0,0,-force));
    
    //Python utick event.
    NGF_PY_CALL_EVENT(utick, evt.timeSinceLastFrame);
}
//-------------------------------------------------------------------------------
void Player::pausedTick(const Ogre::FrameEvent &evt)
{
    //Python ptick event.
    NGF_PY_CALL_EVENT(ptick, evt.timeSinceLastFrame);
}
//-------------------------------------------------------------------------------
NGF::MessageReply Player::receiveMessage(NGF::Message msg)
{
    switch (msg.code)
    {
        case MSG_KEYPRESSED:
            if (GlbVar.paused)
                break;

            switch (msg.getParam<OIS::KeyCode>(0))
            {
                //Dimension switch!
                case OIS::KC_Z:
                    GlbVar.dimMgr->switchDimension();
                    setDimension(GlbVar.dimMgr->getCurrentDimension());
                    break;
            }

            break;
    }

    return GraLL2GameObject::receiveMessage(msg);
}
//-------------------------------------------------------------------------------
void Player::collide(GameObject *other, btCollisionObject *otherPhysicsObject, btManifoldPoint &contact)
{
    //Python collide event.
    NGF::Python::PythonGameObject *oth = dynamic_cast<NGF::Python::PythonGameObject*>(other);
    if (oth)
        NGF_PY_CALL_EVENT(collide, oth->getConnector());
}
//-------------------------------------------------------------------------------

//--- Python interface implementation -------------------------------------------
/*
NGF_PY_BEGIN_IMPL(Player)
{
}
*/
