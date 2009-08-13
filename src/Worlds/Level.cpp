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
    //If higher than highest level, then highest level is this (user went to new level).
    unsigned int worldInd = GlbVar.woMgr->getCurrentWorldIndex();
    if (worldInd > GlbVar.records.highestLevelIndex)
        GlbVar.records.highestLevelIndex = worldInd;

    //Load the level.
    LOG("On to level: " + Ogre::StringConverter::toString(worldInd - GlbVar.firstLevel + 1) + ", NGF: " + mNgfName + ", Caption: " + mCaption);

    //If we're loading games, and savefile exists, load it.
    if (GlbVar.loadGame && saveExists(mNgfName))
    {
        deserialise(saveName(mWorldNum));
    }
    else
    {
        loadLevel(mNgfName);
    }
}
//-------------------------------------------------------------------------------
void Level::tick(const Ogre::FrameEvent &evt)
{
}
//-------------------------------------------------------------------------------
void Level::stop()
{
    clearLevel();
}
//-------------------------------------------------------------------------------

