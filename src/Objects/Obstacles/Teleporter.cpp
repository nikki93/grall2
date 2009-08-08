/*
=========================================
Teleporter.cpp
=========================================
*/

#define __TELEPORTER_CPP__

#include "Objects/Obstacles/Teleporter.h"

#include "Objects/Misc/ParticleEffect.h"

#define TELEPORT_DELAY 0.08

//--- NGF events ----------------------------------------------------------------
Teleporter::Teleporter(Ogre::Vector3 pos, Ogre::Quaternion rot, NGF::ID id, NGF::PropertyList properties, Ogre::String name)
    : NGF::GameObject(pos, rot, id , properties, name),
      mTime(0),
      mPlayer(0)
{
    addFlag("Teleporter");

    //Save on, off events.
    NGF_PY_SAVE_EVENT(teleported);

    //Python init event.
    NGF_PY_CALL_EVENT(init);

    //Get properties.
    Ogre::String pointName = mProperties.getValue("targetPoint", 0, "noname");
    NGF::GameObject *point = GlbVar.goMgr->getByName(pointName);

    if (point)
        mTarget = GlbVar.goMgr->sendMessageWithReply<Ogre::Vector3>(point, NGF_MESSAGE(MSG_GETPOSITION));
    else
        mTarget = Ogre::Vector3(
                Ogre::StringConverter::parseReal(mProperties.getValue("target", 0, "0")),
                Ogre::StringConverter::parseReal(mProperties.getValue("target", 1, "5")),
                Ogre::StringConverter::parseReal(mProperties.getValue("target", 2, "0"))
                );

    //Create the Ogre stuff.
    mEntity = GlbVar.ogreSmgr->createEntity(mOgreName, "Template_Pad.mesh");
    mEntity->setMaterialName("Objects/SwitchOn");
    mNode = GlbVar.ogreSmgr->getRootSceneNode()->createChildSceneNode(mOgreName, pos, rot);
    mNode->attachObject(mEntity);

    //Create the Physics stuff.
    mShape = new btBoxShape(btVector3(0.5,0.25,0.5));

    BtOgre::RigidBodyState *state = new BtOgre::RigidBodyState(mNode);
    mBody = new btRigidBody(0, state, mShape, btVector3(0,0,0));
    initBody();
}
//-------------------------------------------------------------------------------
void Teleporter::postLoad()
{
    //Python create event.
    NGF_PY_CALL_EVENT(create);
}
//-------------------------------------------------------------------------------
Teleporter::~Teleporter()
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
void Teleporter::unpausedTick(const Ogre::FrameEvent &evt)
{
    GraLL2GameObject::unpausedTick(evt);

    //Time flies! :-)
    if (mTime > 0)
        mTime -= evt.timeSinceLastFrame;
    else if (mTime < 0)
    {
        Ogre::Vector3 playerPos = GlbVar.goMgr->sendMessageWithReply<Ogre::Vector3>(mPlayer, NGF_MESSAGE(MSG_GETPOSITION));
        Ogre::Vector3 offset = playerPos - mNode->getPosition(); //Our origin is 0.25 below surface, we want to find offset from surface.
        Ogre::Real yShift = offset.y - 0.75;
        GlbVar.goMgr->sendMessage(mPlayer, NGF_MESSAGE(MSG_TELEPORT, mTarget + Ogre::Vector3(0,yShift,0)));
        mTime = 0;
        mPlayer = 0;
    }

    //Python utick event.
    NGF_PY_CALL_EVENT(utick, evt.timeSinceLastFrame);
}
//-------------------------------------------------------------------------------
void Teleporter::pausedTick(const Ogre::FrameEvent &evt)
{
    //Python ptick event.
    NGF_PY_CALL_EVENT(ptick, evt.timeSinceLastFrame);
}
//-------------------------------------------------------------------------------
NGF::MessageReply Teleporter::receiveMessage(NGF::Message msg)
{
    return GraLL2GameObject::receiveMessage(msg);
}
//-------------------------------------------------------------------------------
void Teleporter::collide(GameObject *other, btCollisionObject *otherPhysicsObject, btManifoldPoint &contact)
{
    if (!other)
        return;

    if (mTime == 0 && other->hasFlag("Player"))
    {
        mTime = TELEPORT_DELAY;
        GlbVar.fader->fadeInOut(TELEPORT_DELAY, 0, TELEPORT_DELAY);
        mPlayer = other;
    }
    //Python collide event.
    NGF::Python::PythonGameObject *oth = dynamic_cast<NGF::Python::PythonGameObject*>(other);
    if (oth)
        NGF_PY_CALL_EVENT(collide, oth->getConnector());
}
//-------------------------------------------------------------------------------

//--- Python interface implementation -------------------------------------------
NGF_PY_BEGIN_IMPL(Teleporter)
{
    NGF_PY_PROPERTY_IMPL(target, mTarget, Ogre::Vector3);
}
NGF_PY_END_IMPL_BASE(GraLL2GameObject)
