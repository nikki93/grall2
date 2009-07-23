/*
=========================================
Level.cpp
=========================================
*/

#include "Worlds/Level.h"

//-------------------------------------------------------------------------------
Level::Level(Ogre::String levelName)
    : mLevelName(levelName)
{
}
//-------------------------------------------------------------------------------
void Level::init()
{
    //Assign some globals.
    GlbVar.levelName = mLevelName;

    //Load the level.
    LOG("Loading level: " + mLevelName);

    //If we're loading games, and savefile exists, load it.
    if (GlbVar.loadGame && saveExists(mLevelName))
    {
        deserialise(mLevelName);
    }
    else
    {
        loadLevel(mLevelName);
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

