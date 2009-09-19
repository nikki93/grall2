/*
=========================================
Level.cpp
=========================================
*/

#include "Worlds/Level.h"

#define OPENING_TEXT_TIME 5
#define OPENING_TEXT_FADE_TIME 2

//-------------------------------------------------------------------------------
Level::Level(unsigned int worldNum, Ogre::String ngfName, Ogre::String caption)
    : mNgfName(ngfName),
      mCaption(caption),
      mWorldNum(worldNum)
{
}
//-------------------------------------------------------------------------------
void Level::init()
{
    unsigned int levelInd = Util::worldNumToLevelNum(mWorldNum);

    //Just for fun. :P
    LOG("On to level: " + Ogre::StringConverter::toString(Util::worldNumToLevelNum(mWorldNum)) + ", NGF: " + mNgfName + ", Caption: " + mCaption + "!");

    //No stuff blocking our view.
    GlbVar.gui->hidePointer();

    //If higher than highest level, then highest level is this (user went to new level).
    if (mWorldNum > GlbVar.records.highestLevelIndex)
        GlbVar.records.highestLevelIndex = mWorldNum;

    //Create the GameObjects!
    if (GlbVar.loadGame && Util::getRecordFromLevelNum(Util::worldNumToLevelNum(mWorldNum)).checkpoint)
    {
        //If we're loading games, and savefile exists, load it.
        Util::deserialise(Util::saveName(mWorldNum));
    }
    else
    {
        //Otherwise read in the level from the .ngf file.
        Util::loadNgf(mNgfName);
        GlbVar.goMgr->sendMessage(GlbVar.controller, NGF_MESSAGE(MSG_LEVELSTART));
    }

    //To save from first frame evt.timeSinceLastFrame spikes.
    GlbVar.paused = true;
}
//-------------------------------------------------------------------------------
void Level::tick(const Ogre::FrameEvent &evt)
{
    //Part of the anti evt.timeSinceLastFrame hack.
    GlbVar.paused = false;
    
    //Some key stuff.
    if (Util::isKeyDown(OIS::KC_N))
        winLevel();
    if (Util::isKeyDown(OIS::KC_ESCAPE))
    {
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
}
//-------------------------------------------------------------------------------

