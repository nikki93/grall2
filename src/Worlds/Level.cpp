/*
=========================================
Level.cpp
=========================================
*/

#include "Worlds/Level.h"

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
    GlbVar.bonusTime = 100;
    GlbVar.gui->hidePointer();
    GlbVar.dimMgr->setDimension(DimensionManager::DIM_1);

    //If higher than highest level, then highest level is this (user went to new level).
    unsigned int worldInd = GlbVar.woMgr->getCurrentWorldIndex();
    if (worldInd > GlbVar.records.highestLevelIndex)
        GlbVar.records.highestLevelIndex = worldInd;

    //Load the level.
    LOG("On to level: " + Ogre::StringConverter::toString(Util::worldNumToLevelNum(worldInd)) + ", NGF: " + mNgfName + ", Caption: " + mCaption);

    //If we're loading games, and savefile exists, load it.
    if (GlbVar.loadGame && Util::getRecordFromLevelNum(Util::worldNumToLevelNum(mWorldNum)).checkpoint)
    {
        Util::deserialise(Util::saveName(mWorldNum));
    }
    else
    {
        Util::loadLevel(mNgfName);
        GlbVar.goMgr->sendMessage(GlbVar.controller, NGF_MESSAGE(MSG_LEVELSTART));
    }
}
//-------------------------------------------------------------------------------
void Level::tick(const Ogre::FrameEvent &evt)
{
    GlbVar.bonusTime = GlbVar.bonusTime > 0 ? GlbVar.bonusTime - evt.timeSinceLastFrame : 0;

    //LOG("Bonus: " + Ogre::StringConverter::toString(GlbVar.bonusTime));
    
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

