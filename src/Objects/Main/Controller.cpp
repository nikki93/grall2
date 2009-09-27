/*
=========================================
Controller.cpp
=========================================
*/

#define __CONTROLLER_CPP__

#include "Objects/Main/Controller.h"

#include "Worlds/Level.h"

//--- NGF events ----------------------------------------------------------------
Controller::Controller(Ogre::Vector3 pos, Ogre::Quaternion rot, NGF::ID id, NGF::PropertyList properties, Ogre::String name)
    : NGF::GameObject(pos, rot, id , properties, name),
      mWin(false),
      mEndCountDown(-1),
      mLevelText(0)
{
    addFlag("Controller");

    //Load script.
    NGF::Python::PythonGameObject::setScript(mProperties.getValue("script", 0, ""));
    NGF_PY_SAVE_EVENT(levelStart);
    NGF_PY_SAVE_EVENT(levelStop);
    NGF_PY_SAVE_EVENT(winLevel);
    NGF_PY_SAVE_EVENT(loseLevel);
    NGF_PY_SAVE_EVENT(alarm);

    //Init.
    NGF_PY_CALL_EVENT(init);

    //Old guy, out!
    if (GlbVar.controller)
        GlbVar.goMgr->destroyObject(GlbVar.controller->getID());
    GlbVar.controller = this;

    //We stick around. :-)
    //setPersistent(true);
}
//-------------------------------------------------------------------------------
Controller::~Controller()
{
    GlbVar.controller = NULL;

    if (mLevelText)
        GlbVar.gui->destroyWidget(mLevelText);
}
//-------------------------------------------------------------------------------
void Controller::unpausedTick(const Ogre::FrameEvent &evt)
{
    //Time flies! :P
    GlbVar.bonusTime = GlbVar.bonusTime > 0 ? GlbVar.bonusTime - evt.timeSinceLastFrame : 0;

    //The win/lose fade thing.
    if (mEndCountDown > 777)
    {
        mEndCountDown -= evt.timeSinceLastFrame;

        //We either restart or go to next depending on whether we won. If user level, go to main menu.
        if (mEndCountDown < 777)
        {
            if (mWin)
            {
                if (Util::worldNumToLevelNum(GlbVar.woMgr->getCurrentWorldIndex()))
                {
                    Util::nextWorld();
                }
                else
                {
                    Util::gotoWorld(0);
                }
            }
            else
            {
                Util::gotoWorld(GlbVar.woMgr->getCurrentWorldIndex());
            }
        }
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

    //The level text stuff.
    if (mLevelText)
    {
        //To safeguard from loading time evt.timeSinceLastFrame spikes.
        /*
        if (mLevelText->getAlpha() == 0)
        {
            mLevelText->setAlpha(0.0001);
        }
        else
        */
        {
            if (mLevelText->getAlpha() < 1)
                mLevelText->setAlpha(Util::clamp(mLevelText->getAlpha() + mLevelTextRate * evt.timeSinceLastFrame, 0.0f, 1.0f));
            else if (mLevelTextPause > 0)
                mLevelTextPause -= evt.timeSinceLastFrame;
            else
                mLevelText->setAlpha(Util::clamp(mLevelText->getAlpha() + (mLevelTextRate = -1/mLevelTextOut) * evt.timeSinceLastFrame, 0.0f, 1.0f));
        }

        if (mLevelText->getAlpha() == 0)
        {
            GlbVar.gui->destroyWidget(mLevelText);
            mLevelText = 0;
        }
    }

    //Update alarms (from AlarmGameObject).
    updateAlarms(evt.timeSinceLastFrame);
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
            NGF_PY_CALL_EVENT(levelStart);
            NGF_NO_REPLY();

        case MSG_LEVELSTOP:
            NGF_PY_CALL_EVENT(levelStop);

            if (mLevelText)
                GlbVar.gui->destroyWidget(mLevelText);
            mLevelText = 0;

            NGF_NO_REPLY();

        case MSG_WINLEVEL:
            mWin = true;
            mEndCountDown = 3;
            NGF_PY_CALL_EVENT(winLevel);
            NGF_NO_REPLY();

        case MSG_LOSELEVEL:
            mWin = false;
            mEndCountDown = 2;
            NGF_PY_CALL_EVENT(loseLevel);
            NGF_NO_REPLY();
    }

    NGF_NO_REPLY();
}
//-------------------------------------------------------------------------------

//--- Python interface implementation -------------------------------------------
NGF_PY_BEGIN_IMPL(Controller)
{
    NGF_PY_METHOD_IMPL(showLevelText)
    {
        if (mLevelText)
            NGF_PY_RETURN();

        mLevelTextRate = 1.0/(py::extract<Ogre::Real>(args[0]));
        mLevelTextPause = py::extract<Ogre::Real>(args[1]);
        mLevelTextOut = py::extract<Ogre::Real>(args[2]);

        unsigned int level = Util::worldNumToLevelNum(GlbVar.woMgr->getCurrentWorldIndex());
        Ogre::String caption = static_cast<Level*>((GlbVar.woMgr->getWorld(GlbVar.woMgr->getCurrentWorldIndex())))->getCaption();

        int w = GlbVar.ogreWindow->getWidth();
        int h = GlbVar.ogreWindow->getHeight();
        mLevelText = GlbVar.gui->createWidget<MyGUI::StaticText>("StaticText", 0, (int)(0.25 * h - 25), w, 50, MyGUI::Align::Default, "Popup");
        mLevelText->setTextAlign(MyGUI::Align::HCenter | MyGUI::Align::VCenter);
        mLevelText->setCaption((level ? "Level " + Ogre::StringConverter::toString(level) : "User Level") + " - " + caption);
        mLevelText->setFontName("BigFont");
        mLevelText->setAlpha(0);
        NGF_PY_RETURN();
    }

    //setAlarm
    GRALL2_PY_ALARM_METHOD(setAlarm);
}
NGF_PY_END_IMPL
//-------------------------------------------------------------------------------
