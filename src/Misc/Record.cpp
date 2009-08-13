/*
 * =====================================================================================
 *
 *       Filename:  Record.cpp
 *
 *    Description:  Saving/loading of user record (highest level reached, scores etc.)
 *
 *        Created:  Thursday 13 August 2009 02:49:36  IST
 *       Compiler:  gcc
 *
 *         Author:  Nikhilesh (nikki)
 *
 * =====================================================================================
 */

#include "Globals.h"

#define RECORD_FILE (USER_PREFIX + "Record")

void loadRecord()
{
    Ogre::ConfigFile cfg;
    Ogre::String fileName = USER_PREFIX + "Record";

    try
    {
        cfg.loadDirect(RECORD_FILE);
    }
    catch (Ogre::FileNotFoundException &e)
    {
        std::ofstream out(RECORD_FILE.c_str());
        out << std::endl;
        out.close();

        cfg.loadDirect(RECORD_FILE);
    }

    Ogre::String highestLevelStr = cfg.getSetting("highestLevelIndex", Ogre::StringUtil::BLANK, "n");

    if (highestLevelStr != "n")
        GlbVar.records.highestLevelIndex = Ogre::StringConverter::parseInt(highestLevelStr);
    else
        GlbVar.records.highestLevelIndex = GlbVar.firstLevel;
}

void saveRecord()
{
    std::ofstream cfg(RECORD_FILE.c_str());

    cfg << "highestLevelIndex = " << GlbVar.records.highestLevelIndex;
    cfg << std::endl;

    cfg.close();
}

void clearRecord()
{
    remove(RECORD_FILE.c_str());
    loadRecord();
}
