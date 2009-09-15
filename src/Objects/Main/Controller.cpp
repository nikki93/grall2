/*
=========================================
Controller.cpp
=========================================
*/

#define __CONTROLLER_CPP__

#include "Objects/Main/Controller.h"

//--- NGF events ----------------------------------------------------------------
Controller::Controller(Ogre::Vector3 pos, Ogre::Quaternion rot, NGF::ID id, NGF::PropertyList properties, Ogre::String name)
    : NGF::GameObject(pos, rot, id , properties, name),
      mWin(false),
      mEndCountDown(-1)
{
    addFlag("Controller");

    //Load script.
    NGF::Python::PythonGameObject::setScript(mProperties.getValue("script", 0, ""));
    NGF_PY_SAVE_EVENT(levelStart);
    NGF_PY_SAVE_EVENT(levelStop);
    NGF_PY_SAVE_EVENT(winLevel);
    NGF_PY_SAVE_EVENT(loseLevel);

    //Init.
    NGF_PY_CALL_EVENT(init);

    //Old guy, out!
    if (GlbVar.controller)
        GlbVar.goMgr->destroyObject(GlbVar.controller->getID());
    GlbVar.controller = this;

    //We stick around. :-)
    setPersistent(true);
}
//-------------------------------------------------------------------------------
Controller::~Controller()
{
    GlbVar.controller = NULL;
}
//-------------------------------------------------------------------------------
void Controller::unpausedTick(const Ogre::FrameEvent &evt)
{
    //Stupid hack for two-stage timer (first is until fade, then after fade).
    if (mEndCountDown > 777)
    {
        mEndCountDown -= evt.timeSinceLastFrame;

        //We either restart or go to next depending on whether we won.
        if (mEndCountDown < 777)
            if (mWin)
                Util::nextWorld();
            else
                Util::gotoWorld(GlbVar.woMgr->getCurrentWorldIndex());
    }
    else if (mEndCountDown > 0)
    {
        mEndCountDown -= evt.timeSinceLastFrame;

        //Sart the timer and go into second stage.
        if (mEndCountDown < 0)
        {
            GlbVar.fader->fadeInOut(0.7,0.4,0.5);
            mEndCountDown = 778.1;
        }
    }
}
//-------------------------------------------------------------------------------
void Controller::postLoad()
{
    //Create.
    NGF_PY_CALL_EVENT(create);
}
//-------------------------------------------------------------------------------
NGF::MessageReply Controller::receiveMessage(NGF::Message msg)
{
    switch (msg.code)
    {
        case MSG_LEVELSTART:
            mPythonEvents["levelStart"](); //No 'self' parameter.
            NGF_NO_REPLY();

        case MSG_LEVELSTOP:
            mPythonEvents["levelStop"]();
            NGF_NO_REPLY();

        case MSG_WINLEVEL:
            mWin = true;
            mEndCountDown = 3;
            mPythonEvents["winLevel"]();
            NGF_NO_REPLY();

        case MSG_LOSELEVEL:
            mWin = false;
            mEndCountDown = 2;
            mPythonEvents["loseLevel"]();
            NGF_NO_REPLY();
    }

    NGF_NO_REPLY();
}
//-------------------------------------------------------------------------------
