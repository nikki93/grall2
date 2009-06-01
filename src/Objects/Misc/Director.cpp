/*
=========================================
Director.cpp
=========================================
*/

#define __Director_CPP__

#include "Objects/Misc/Director.h"

//--- NGF events ----------------------------------------------------------------
Director::Director(Ogre::Vector3 pos, Ogre::Quaternion rot, NGF::ID id, NGF::PropertyList properties, Ogre::String name)
    : NGF::GameObject(pos, rot, id , properties, name)
{
    addFlag("Director");

    //Python init event.
    NGF_PY_CALL_EVENT(init);

    //Create the Ogre stuff.
    mNode = GlbVar.ogreSmgr->getRootSceneNode()->createChildSceneNode(mOgreName, pos, rot);

    //Save properties.
    mVelocity = Ogre::Vector3(0,0,-(Ogre::StringConverter::parseReal(mProperties.getValue("speed", 0, "2"))));
    mDirection = rot;

    //Create the Physics stuff.
    mShape = new btBoxShape(btVector3(0.5,0.5,0.5));
    BtOgre::RigidBodyState *state = new BtOgre::RigidBodyState(mNode);
    mBody = new btRigidBody(0, state, mShape, btVector3(0,0,0));
    GlbVar.phyWorld->addRigidBody(mBody, mDimensions | DimensionManager::NO_DIM_CHECK, mDimensions);
    setBulletObject(mBody);
    mBody->setCollisionFlags(mBody->getCollisionFlags() | btCollisionObject::CF_NO_CONTACT_RESPONSE);
}
//-------------------------------------------------------------------------------
void Director::postLoad()
{
    //Python create event.
    NGF_PY_CALL_EVENT(create);
}
//-------------------------------------------------------------------------------
Director::~Director()
{
    //Python destroy event.
    NGF_PY_CALL_EVENT(destroy);

    //We only clear up stuff that we did.
    destroyBody();
    delete mShape;
}
//-------------------------------------------------------------------------------
void Director::unpausedTick(const Ogre::FrameEvent &evt)
{
    GraLL2GameObject::unpausedTick(evt);
    
    //Python utick event.
    NGF_PY_CALL_EVENT(utick, evt.timeSinceLastFrame);
}
//-------------------------------------------------------------------------------
void Director::pausedTick(const Ogre::FrameEvent &evt)
{
    //Python ptick event.
    NGF_PY_CALL_EVENT(ptick, evt.timeSinceLastFrame);
}
//-------------------------------------------------------------------------------
NGF::MessageReply Director::receiveMessage(NGF::Message msg)
{
    switch (msg.code)
    {
        case MSG_GETVELOCITY:
            NGF_SEND_REPLY(mDirection * mVelocity);

        case MSG_GETPOSITION:
            NGF_SEND_REPLY(mNode->getPosition());
    }
    return GraLL2GameObject::receiveMessage(msg);
}
//-------------------------------------------------------------------------------
void Director::collide(GameObject *other, btCollisionObject *otherPhysicsObject, btManifoldPoint &contact)
{
    //Python collide event.
    NGF::Python::PythonGameObject *oth = dynamic_cast<NGF::Python::PythonGameObject*>(other);
    if (oth)
        NGF_PY_CALL_EVENT(collide, oth->getConnector());
}
//-------------------------------------------------------------------------------

//--- Python interface implementation -------------------------------------------
/*
NGF_PY_BEGIN_IMPL(Director)
{
}
NGF_PY_END_IMPL_BASE(GraLL2GameObject)
*/
