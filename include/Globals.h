/*
 * =====================================================================================
 *
 *       Filename:  Globals.h
 *
 *    Description:  Provides a 'Globals' class for easy global access of variables.
 *                  Also has some declarations for global functions.
 *
 *        Created:  04/26/2009 07:39:52 PM
 *       Compiler:  gcc
 *
 *         Author:  Nikhilesh (nikki)
 *
 * =====================================================================================
 */

#include "Precomp.h"

#ifndef __GLOBALS_H__
#define __GLOBALS_H__

//Defines.
#define GlbVar Globals::getSingleton()
#define LOG(msg) Ogre::LogManager::getSingleton().logMessage(msg);

//Forward declarations.
class DimensionManager;

//The Globals class (actually a struct, but whatever).
struct Globals : public Ogre::Singleton<Globals>
{
    //Managers etc.
    //Ogre.
    Ogre::Root *ogreRoot;
    Ogre::SceneManager *ogreSmgr;
    Ogre::RenderWindow *ogreWindow;
    Ogre::Camera *ogreCamera;

    //OIS.
    OIS::Keyboard *keyboard;
    OIS::Mouse *mouse;

    //Bullet.
    btDiscreteDynamicsWorld *phyWorld;

    //MyGUI.
    MyGUI::Gui *gui;

    //NGF.
    NGF::GameObjectManager *goMgr;
    NGF::WorldManager *woMgr;
    NGF::Loading::Loader *lvlLoader;

    //DimensionManager.
    DimensionManager *dimMgr;

    //Globally required data.
    bool paused;
};

//Global functions.
void addWorlds();
void registerGameObjectTypes();

//Messages.
enum
{
    MSG_NONE,
    MSG_DIMENSIONCHANGED,
};

//Includes from the project used everywhere.
#include "Misc/DimensionManager.h"

#endif
