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
#include "Objects/Obstacles/Pickup.h"
#include "Objects/Bots/Bullet.h"
#include "Objects/Bots/Turret.h"
#include "Objects/Misc/Empty.h"
#include "Objects/Obstacles/FallingBrush.h"
#include "Objects/Obstacles/OneWay.h"

//World headers.
#include "Worlds/TestWorld.h"
#include "Worlds/Level.h"
#include "Worlds/MainMenu.h"
#include "Worlds/MaterialViewer.h"

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
    NGF_REGISTER_OBJECT_TYPE(Pickup);
    NGF_REGISTER_OBJECT_TYPE(Bullet);
    NGF_REGISTER_OBJECT_TYPE(Turret);
    NGF_REGISTER_OBJECT_TYPE(Empty);
    NGF_REGISTER_OBJECT_TYPE(FallingBrush);
    NGF_REGISTER_OBJECT_TYPE(OneWay);
}

//Register worlds.
void addWorlds()
{
    Ogre::ConfigFile cfg;
    cfg.loadFromResourceSystem("Levels.ini", "General");

    //Main menu.
    GlbVar.woMgr->addWorld(new MainMenu());
    GlbVar.woMgr->addWorld(new MaterialViewer());
    GlbVar.woMgr->addWorld(new Level(2, "", "", true)); //User level.

    //Levels.
    GlbVar.firstLevel = GlbVar.woMgr->getNumWorlds(); //Size = index + 1.

    unsigned int currNum = GlbVar.firstLevel;
    for (int i = 1; ; ++i)
    {
        Ogre::String attrStr = cfg.getSetting("Level" + Ogre::StringConverter::toString(i));

        if (attrStr == "")
            break;

        Ogre::StringVector attrStrs = Ogre::StringUtil::split(attrStr, "|");
        GlbVar.woMgr->addWorld(new Level(currNum++, attrStrs[0], attrStrs[1]));

        GlbVar.ngfNames.push_back(attrStrs[0]);
    }
}

