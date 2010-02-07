/*
=========================================
Switch.cpp
=========================================
*/

#define __SWITCH_CPP__

#include "Objects/Obstacles/Switch.h"

//--- NGF events ----------------------------------------------------------------
Switch::Switch(Ogre::Vector3 pos, Ogre::Quaternion rot, NGF::ID id, NGF::PropertyList properties, Ogre::String name)
    : NGF::GameObject(pos, rot, id , properties, name),
      mTime(-1),
      mTimePrev(-1),
      mOn(false),
      mOnPrev(false),
      mType(ST_TOGGLE)
{
    addFlag("Switch");

    //Save on, off events.
    NGF_PY_SAVE_EVENT(on);
    NGF_PY_SAVE_EVENT(off);

    //Python init event.
    NGF_PY_CALL_EVENT(init);

    //Get properties.
    mDelay = Ogre::StringConverter::parseReal(mProperties.getValue("delay", 0, "0.2"));

    Ogre::String typeStr = mProperties.getValue("switchType", 0, "toggle");
    if (typeStr == "once")
        mType = ST_ONCE;
    else if (typeStr == "press")
        mType = ST_PRESS;

    //Create the Ogre stuff.
    mEntity = GlbVar.ogreSmgr->createEntity(mOgreName, "Mesh_Pad.mesh");
    mEntity->setMaterialName("Objects/SwitchOff");
    mNode = GlbVar.ogreSmgr->getRootSceneNode()->createChildSceneNode(mOgreName, pos, rot);
    mNode->attachObject(mEntity);

    //Create the Physics stuff.
    mShape = new btBoxShape(btVector3(0.5,0.25,0.5));

    BtOgre::RigidBodyState *state = new BtOgre::RigidBodyState(mNode);
    mBody = new btRigidBody(0, state, mShape, btVector3(0,0,0));
    initBody();

    //Create sounds.
    mOnSound = GlbVar.soundMgr->createSound(mOgreName + "_onSound", "SwitchOn.wav", false, false);
    mNode->attachObject(mOnSound);
    mOnSound->setReferenceDistance(1.2);
    mOnSound->setGain(0.3);

    mOffSound = GlbVar.soundMgr->createSound(mOgreName + "_offSound", "SwitchOff.wav", false, false);
    mNode->attachObject(mOffSound);
    mOffSound->setReferenceDistance(1.2);
    mOffSound->setGain(5);
}
//-------------------------------------------------------------------------------
void Switch::postLoad()
{
    //Get the SlidingBrush (if there's one).
    mSlidingBrush = GlbVar.goMgr->getByName(mProperties.getValue("slidingBrush", 0, "noname"));

    //Python create event.
    NGF_PY_CALL_EVENT(create);
}
//-------------------------------------------------------------------------------
Switch::~Switch()
{
    //Python destroy event.
    NGF_PY_CALL_EVENT(destroy);

    //We only clear up stuff that we did.
    destroyBody();
    delete mShape;

    mNode->detachAllObjects();
    GlbVar.soundMgr->destroySound(mOnSound);
    GlbVar.soundMgr->destroySound(mOffSound);
    GlbVar.ogreSmgr->destroyEntity(mEntity->getName());
}
//-------------------------------------------------------------------------------
void Switch::unpausedTick(const Ogre::FrameEvent &evt)
{
    GraLL2GameObject::unpausedTick(evt);

    //Handle on/off.
    if (mTime > 0)
        mTime -= evt.timeSinceLastFrame;

    switch (mType)
    {
        case ST_ONCE:
            //Once we're on, we're on.
            if (mTime > 0)
                mOn = true;
            break;

        case ST_PRESS:
            //When on, on, when off, off.
            if (mTime > 0)
                mOn = true;
            else
                mOn = false;
            break;

        case ST_TOGGLE:
            //When 'just' on, toggle.
            if (mTime > 0 && mTimePrev <= 0)
                mOn = !mOn;
            break;
    }

    mTimePrev = mTime;

    //Event calling.
    if (mOn != mOnPrev)
    {
        if (mOn)
            on();
        else
            off();
    }
    mOnPrev = mOn;

    //Material stuff.
    mEntity->setMaterialName(mOn ? "Objects/SwitchOn" : "Objects/SwitchOff");

    //Python utick event.
    NGF_PY_CALL_EVENT(utick, evt.timeSinceLastFrame);
}
//-------------------------------------------------------------------------------
void Switch::pausedTick(const Ogre::FrameEvent &evt)
{
    //Python ptick event.
    NGF_PY_CALL_EVENT(ptick, evt.timeSinceLastFrame);
}
//-------------------------------------------------------------------------------
NGF::MessageReply Switch::receiveMessage(NGF::Message msg)
{
    return GraLL2GameObject::receiveMessage(msg);
}
//-------------------------------------------------------------------------------
void Switch::collide(GameObject *other, btCollisionObject *otherPhysicsObject, btManifoldPoint &contact)
{
    if (!other)
        return;

    if (other->hasFlag("Switcher"))
        mTime = mDelay;

    //Python collide event.
    NGF::Python::PythonGameObject *oth = dynamic_cast<NGF::Python::PythonGameObject*>(other);
    if (oth)
        NGF_PY_CALL_EVENT(collide, oth->getConnector());
}
//-------------------------------------------------------------------------------

//--- Non-NGF -------------------------------------------------------------------
void Switch::on()
{
    if (mSlidingBrush)
        GlbVar.goMgr->sendMessage(mSlidingBrush, NGF_MESSAGE(MSG_SETFORWARD, (bool) true));

    mOnSound->stop();
    mOnSound->play();

    NGF_PY_CALL_EVENT(on);
}
//-------------------------------------------------------------------------------
void Switch::off()
{
    if (mSlidingBrush)
        GlbVar.goMgr->sendMessage(mSlidingBrush, NGF_MESSAGE(MSG_SETFORWARD, (bool) false));

    mOffSound->stop();
    mOffSound->play();

    NGF_PY_CALL_EVENT(off);
}
//-------------------------------------------------------------------------------

//--- Python interface implementation -------------------------------------------
NGF_PY_BEGIN_IMPL(Switch)
{
    NGF_PY_PROPERTY_IMPL(on, mOn, bool);
    NGF_PY_PROPERTY_IMPL(type, mType, int);
}
NGF_PY_END_IMPL_BASE(GraLL2GameObject)
