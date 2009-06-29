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
    if (GlbVar.loadGame && std::ifstream((USER_PREFIX + "Saves/" + mLevelName).c_str()))
    {
        LOG("LOADING");
        NGF::Serialisation::Serialiser::load(USER_PREFIX + "Saves/" + mLevelName);
    }
    else
    {
        loadLevel(mLevelName);
        //NGF::Serialisation::Serialiser::save(USER_PREFIX + "Saves/" + mLevelName);
    }
}
//-------------------------------------------------------------------------------
void Level::tick(const Ogre::FrameEvent &evt)
{
    if (isKeyDown(OIS::KC_N))
        NGF::Serialisation::Serialiser::save(USER_PREFIX + "Saves/" + mLevelName);
}
//-------------------------------------------------------------------------------
void Level::stop()
{
    clearLevel();
}
//-------------------------------------------------------------------------------

