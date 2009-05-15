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
class Console;

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

    //Python console window.
    Console *console;

    //Current CameraHandler.
    NGF::GameObject *currCameraHandler;

    //Globally required data.
    bool paused;
};

//'Procedures'.
void addWorlds();
void registerGameObjectTypes();

//Messages.
enum
{
    MSG_NONE,                 //No message.
    MSG_DIMENSIONCHANGED,     //Dimension changed.
    MSG_KEYPRESSED,           //Key pressed.
    MSG_SETTARGET,            //Set CameraHandler target node.
    MSG_SETOFFSET,            //Set CameraHandler third-person camera offset.
    MSG_SETSMOOTHINGFACTOR,   //Set CameraHandler camera movement smoothing.
    MSG_SETCAMERASTATE,       //Set CameraHandler camera handling mode.
};

//Includes from the project used everywhere.
#include "Misc/DimensionManager.h"
#include "Misc/Console.h"
#include "Misc/Utils.h"

#endif
