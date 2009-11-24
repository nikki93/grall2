/*
=========================================
Level.cpp
=========================================
*/

#include "Worlds/Level.h"

#define OPENING_TEXT_TIME 5
#define OPENING_TEXT_FADE_TIME 2

//-------------------------------------------------------------------------------
Level::Level(unsigned int worldNum, Ogre::String ngfName, Ogre::String caption, bool userLevel)
    : mNgfName(ngfName),
      mCaption(caption),
      mWorldNum(worldNum),
      mUserLevel(userLevel)
{
    if (mUserLevel)
        mNgfName = "NULL";
}
//-------------------------------------------------------------------------------
void Level::init()
{
    //Just for fun. :P
    LOG(FORMAT("On to level: %d, NGF: %s, Caption: %s!", Util::worldNumToLevelNum(mWorldNum) % mNgfName % mCaption));

    //If higher than highest level, then highest level is this (user went to new level).
    if (mWorldNum > GlbVar.records.highestLevelIndex)
        GlbVar.records.highestLevelIndex = mWorldNum;

    if (mUserLevel && GlbVar.records.highestLevelIndex == mWorldNum)
        GlbVar.newLevel = true;
    else
        GlbVar.newLevel = false;

    //Create default Pickup counters.
    GlbVar.hud->addPickupDisplay("KeyR", Ogre::ColourValue(0.8, 0.2, 0.2));
    GlbVar.hud->addPickupDisplay("KeyB", Ogre::ColourValue(0.2, 0.2, 0.8));
    GlbVar.hud->addPickupDisplay("KeyG", Ogre::ColourValue(0.2, 0.8, 0.2));
    GlbVar.hud->addPickupDisplay("KeyY", Ogre::ColourValue(0.8, 0.6, 0.2));

    //Create the GameObjects! If null name, user level not chosen yet, just skip.
    if (mNgfName != "NULL")
        startLevel();
    else
        Util::nextWorld();
}
//-------------------------------------------------------------------------------
void Level::tick(const Ogre::FrameEvent &evt)
{
    //Part of the anti evt.timeSinceLastFrame hack.
    if (mFirstFrame)
    {
        GlbVar.paused = false;
        mFirstFrame = false;
    }
    
    //Some key stuff.
    if (Util::isKeyDown(OIS::KC_N))
        winLevel();
    if (Util::isKeyDown(OIS::KC_ESCAPE))
    {
        if (mUserLevel)
            mNgfName = "NULL";

        //In case some fades have been started, we stop 'em.
        GlbVar.fader->abortFade(0);
        Util::gotoWorld(0);
    }
}
//-------------------------------------------------------------------------------
void Level::stop()
{
    GlbVar.goMgr->sendMessage(GlbVar.controller, NGF_MESSAGE(MSG_LEVELSTOP));
    Util::clearLevel();
    GlbVar.hud->clear();
}
//-------------------------------------------------------------------------------
void Level::startLevel()
{
    //First we're not paused.
    GlbVar.paused = false;

    //No stuff blocking our view.
    GlbVar.gui->hidePointer();

    //If there's a checkpoint, and we're loading games, load it.
    if (!(GlbVar.loadGame && Util::loadCheckpoint()))
    {
        //Otherwise read in the level from the .ngf file.
        Util::loadNgf(mNgfName);
        GlbVar.goMgr->sendMessage(GlbVar.controller, NGF_MESSAGE(MSG_LEVELSTART));
    }

    //To save from load-frame evt.timeSinceLastFrame spikes. If some GameObject paused it already,
    //then we skip it.
    mFirstFrame = !GlbVar.paused;
    GlbVar.paused = true;
}
//-------------------------------------------------------------------------------
