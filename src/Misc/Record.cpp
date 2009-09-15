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

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/map.hpp>

#include "Objects/Misc/CameraHandler.h"

#define RECORD_FILE (USER_PREFIX "Record")

void loadRecord()
{
    Ogre::ConfigFile cfg;

    try
    {
        cfg.loadDirect(RECORD_FILE);
    }
    catch (Ogre::FileNotFoundException &e)
    {
        std::ofstream out(RECORD_FILE);
        out << std::endl;
        out.close();

        cfg.loadDirect(RECORD_FILE);
    }

    GlbVar.records.firstTime = Ogre::StringConverter::parseBool(cfg.getSetting("firstTime", Ogre::StringUtil::BLANK, "yes"));

    Ogre::String highestLevelStr = cfg.getSetting("highestLevelIndex", Ogre::StringUtil::BLANK, "n");
    if (highestLevelStr != "n")
        GlbVar.records.highestLevelIndex = Ogre::StringConverter::parseInt(highestLevelStr);
    else
        GlbVar.records.highestLevelIndex = GlbVar.firstLevel;

    //Deserialise recordMap from string.
    Ogre::String recordMapStr = cfg.getSetting("records", Ogre::StringUtil::BLANK, "n");
    if (recordMapStr != "n")
    {
        std::stringstream recordMapStream(std::stringstream::in | std::stringstream::out);
        recordMapStream << recordMapStr;
        boost::archive::text_iarchive ia(recordMapStream);
        ia >> GlbVar.records.recordMap;
    }
    else
    {
        GlbVar.records.recordMap.clear();
    }
}

void saveRecord()
{
    std::ofstream cfg(RECORD_FILE);

    cfg << "firstTime = " << GlbVar.records.firstTime << std::endl;
    cfg << "highestLevelIndex = " << GlbVar.records.highestLevelIndex << std::endl;

    //Serialise recordMap to string.
    Ogre::String recordMapStr;
    std::stringstream recordMapStream(std::stringstream::in | std::stringstream::out);
    boost::archive::text_oarchive oa(recordMapStream);
    oa << GlbVar.records.recordMap;
    recordMapStr = recordMapStream.str();

    cfg << "records = " << recordMapStr;

    cfg.close();
}

void clearRecord()
{
    remove(RECORD_FILE);
    loadRecord();
}

void loseLevel()
{
    //Do the Camera thing.
    GlbVar.goMgr->sendMessage(GlbVar.player, NGF_MESSAGE(MSG_CAPTURECAMERAHANDLER));
    GlbVar.goMgr->sendMessage(GlbVar.currCameraHandler, NGF_MESSAGE(MSG_SETCAMERASTATE, int(CameraHandler::CS_DEATH)));

    //Set record stuff.
    unsigned int levelNum = Util::worldNumToLevelNum(GlbVar.woMgr->getCurrentWorldIndex());
    Globals::Records::Record &rec = Util::getRecordFromLevelNum(levelNum);

    ++rec.losses; //Increment losses.

    //Do the fade and restart.
    GlbVar.goMgr->sendMessage(GlbVar.controller, NGF_MESSAGE(MSG_LOSELEVEL));
}

void winLevel()
{
    //Record stuff.
    unsigned int levelNum = Util::worldNumToLevelNum(GlbVar.woMgr->getCurrentWorldIndex());
    Globals::Records::Record &rec = Util::getRecordFromLevelNum(levelNum);

    rec.completed = true; //Completed!
    if (rec.score < GlbVar.bonusTime) //Save better score.
        rec.score = GlbVar.bonusTime;

    //Do the fade and go to next level.
    GlbVar.goMgr->sendMessage(GlbVar.controller, NGF_MESSAGE(MSG_WINLEVEL));
}
