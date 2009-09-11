/*
 * =====================================================================================
 *
 *       Filename:  Lists.cpp
 *
 *    Description:  Has the World and GameObject 'lists', for registration.
 *
 *        Created:  04/28/2009 03:59:06 PM
 *       Compiler:  gcc
 *
 *         Author:  Nikhilesh (nikki)
 *
 * =====================================================================================
 */

#include "Globals.h"

//GameObject headers.
#include "Objects/Main/Player.h"
#include "Objects/Main/StaticBrush.h"
#include "Objects/Misc/Light.h"
#include "Objects/Misc/CameraHandler.h"
#include "Objects/Misc/Point.h"
#include "Objects/Misc/Trigger.h"
#include "Objects/Misc/RigidBody.h"
#include "Objects/Obstacles/MovingBrush.h"
#include "Objects/Misc/Director.h"
#include "Objects/Obstacles/Crate.h"
#include "Objects/Misc/MessageBox.h"
#include "Objects/Obstacles/Bomb.h"
#include "Objects/Misc/ParticleEffect.h"
#include "Objects/Obstacles/Ice.h"
#include "Objects/Obstacles/SlidingBrush.h"
#include "Objects/Misc/Checkpoint.h"
#include "Objects/Obstacles/MovingBomb.h"
#include "Objects/Obstacles/Switch.h"
#include "Objects/Main/Controller.h"
#include "Objects/Obstacles/Teleporter.h"

//World headers.
#include "Worlds/TestWorld.h"
#include "Worlds/Level.h"
#include "Worlds/MainMenu.h"

//Register GameObject types.
void registerGameObjectTypes()
{
    NGF_REGISTER_OBJECT_TYPE(Player);
    NGF_REGISTER_OBJECT_TYPE(StaticBrush);
    NGF_REGISTER_OBJECT_TYPE(Light);
    NGF_REGISTER_OBJECT_TYPE(CameraHandler);
    NGF_REGISTER_OBJECT_TYPE(Point);
    NGF_REGISTER_OBJECT_TYPE(Trigger);
    NGF_REGISTER_OBJECT_TYPE(RigidBody);
    NGF_REGISTER_OBJECT_TYPE(MovingBrush);
    NGF_REGISTER_OBJECT_TYPE(Director);
    NGF_REGISTER_OBJECT_TYPE(Crate);
    NGF_REGISTER_OBJECT_TYPE(MessageBox);
    NGF_REGISTER_OBJECT_TYPE(Bomb);
    NGF_REGISTER_OBJECT_TYPE(ParticleEffect);
    NGF_REGISTER_OBJECT_TYPE(Ice);
    NGF_REGISTER_OBJECT_TYPE(SlidingBrush);
    NGF_REGISTER_OBJECT_TYPE(Checkpoint);
    NGF_REGISTER_OBJECT_TYPE(MovingBomb);
    NGF_REGISTER_OBJECT_TYPE(Switch);
    NGF_REGISTER_OBJECT_TYPE(Controller);
    NGF_REGISTER_OBJECT_TYPE(Teleporter);
}

//Register worlds.
void addWorlds()
{
    //Main menu.
    GlbVar.woMgr->addWorld(new MainMenu());

    //Levels.
    GlbVar.firstLevel = GlbVar.woMgr->getNumWorlds(); //Size = index + 1.
    
    Ogre::ConfigFile cfg;
    cfg.loadFromResourceSystem("Levels.ini", "General");

    unsigned int currNum = GlbVar.firstLevel;

    for (int i = 1; ; ++i)
    {
        Ogre::String attrStr = cfg.getSetting("Level" + Ogre::StringConverter::toString(i));

        if (attrStr == "")
            break;

        Ogre::StringVector attrStrs = Ogre::StringUtil::split(attrStr, "|");
        GlbVar.woMgr->addWorld(new Level(currNum++, attrStrs[0], attrStrs[1]));
    }

    /*
    std::vector<Ogre::String> lvlStrs = GlbVar.lvlLoader->getLevels();

    int worldNum = GlbVar.woMgr->getNumWorlds();

    for (int i = 1; ; ++i)
    {
        if (lvlStrs.empty())
            break;

        Ogre::String currName = "Level" + Ogre::StringConverter::toString(i);
        std::vector<Ogre::String>::iterator iter = std::find(lvlStrs.begin(), lvlStrs.end(), currName);
        if (iter != lvlStrs.end())
        {
            GlbVar.woMgr->addWorld(new Level(currName));
            ++worldNum;
            GlbVar.levelMap[worldNum] = currName;

            lvlStrs.erase(iter);
        }
    }
    */
}

