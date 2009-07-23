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
      mOn(false),
      mOnPrev(false)
{
    addFlag("Switch");

    //Save on, off events.
    NGF_PY_SAVE_EVENT(on);
    NGF_PY_SAVE_EVENT(off);

    //Python init event.
    NGF_PY_CALL_EVENT(init);

    //Get properties.
    mDelay = Ogre::StringConverter::parseReal(mProperties.getValue("delay", 0, "0.2"));
    mOnce =  Ogre::StringConverter::parseBool(mProperties.getValue("once", 0, "no"));

    //Create the Ogre stuff.
    mEntity = GlbVar.ogreSmgr->createEntity(mOgreName, "Template_Pad.mesh");
    mEntity->setMaterialName("Objects/SwitchOn");
    mNode = GlbVar.ogreSmgr->getRootSceneNode()->createChildSceneNode(mOgreName, pos, rot);
    mNode->attachObject(mEntity);

    //Create the Physics stuff.
    mShape = new btBoxShape(btVector3(0.5,0.25,0.5)); //Overlap a bit for smoothness.

    BtOgre::RigidBodyState *state = new BtOgre::RigidBodyState(mNode);
    mBody = new btRigidBody(0, state, mShape, btVector3(0,0,0));
    initBody();
}
//-------------------------------------------------------------------------------
void Switch::postLoad()
{
    //Get the SlidingBrush (if there's one).
    mSlidingBrushName = mProperties.getValue("slidingBrush", 0, "none");

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
    GlbVar.ogreSmgr->destroyEntity(mEntity->getName());
}
//-------------------------------------------------------------------------------
void Switch::unpausedTick(const Ogre::FrameEvent &evt)
{
    GraLL2GameObject::unpausedTick(evt);

    //Handle on/off.
    if (mTime > 0)
    {
        mTime -= evt.timeSinceLastFrame;
        mOn = true;
    }
    else if (!mOnce)
        mOn = false;

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
    NGF::GameObject *slider = GlbVar.goMgr->getByName(mSlidingBrushName);
    if (slider)
        GlbVar.goMgr->sendMessage(slider, NGF_MESSAGE(MSG_SETFORWARD, (bool) true));

    NGF_PY_CALL_EVENT(on);
}
//-------------------------------------------------------------------------------
void Switch::off()
{
    NGF::GameObject *slider = GlbVar.goMgr->getByName(mSlidingBrushName);
    if (slider)
        GlbVar.goMgr->sendMessage(slider, NGF_MESSAGE(MSG_SETFORWARD, (bool) false));

    NGF_PY_CALL_EVENT(off);
}
//-------------------------------------------------------------------------------

//--- Python interface implementation -------------------------------------------
NGF_PY_BEGIN_IMPL(Switch)
{
    NGF_PY_PROPERTY_IMPL(on, mOn, bool);
    NGF_PY_PROPERTY_IMPL(once, mOnce, bool);
}
NGF_PY_END_IMPL_BASE(GraLL2GameObject)
