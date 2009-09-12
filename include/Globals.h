/*
 * =====================================================================================
 *
 *       Filename:  Globals.h
 *
 *    Description:  Provides a 'Globals' class for easy global access of variables.
 *                  Also has some declarations for global functions.
 *
 *                  NOTE: Dirty code. :-)
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

//Some compile-time settings.
#define NO_LIGHTS //No level-lights (hard-coded lights will stay).

//Defines.
#define GlbVar Globals::getSingleton()
#define LOG(msg) Ogre::LogManager::getSingleton().logMessage(msg);
#define USER_PREFIX Ogre::String("../../usr/")

//Forward declarations.
class DimensionManager;
class Console;
class Fader;
class MusicManager;
class VideoRecorder;
class KeyMap;
class OptionsDialog;
class ObjectClicker;

//The Globals class (actually a struct, but whatever).
struct Globals : public Ogre::Singleton<Globals>
{
    //--- Managers ---------------------------------------------------------------------
    
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
    BtOgre::DebugDrawer *phyDebug;

    //MyGUI.
    MyGUI::Gui *gui;

    //OgreAL.
    OgreAL::SoundManager *soundMgr;

    //NGF.
    NGF::GameObjectManager *goMgr;
    NGF::WorldManager *woMgr;
    NGF::Loading::Loader *lvlLoader;

    //Globally available helpers.
    DimensionManager *dimMgr;
    Console *console;
    Fader *fader;
    MusicManager *musicMgr;
    VideoRecorder *videoRec;
    KeyMap *keyMap;
    OptionsDialog *optionsDialog;
    ObjectClicker *objectClicker;

    //--- Global 'objects' -------------------------------------------------------------

    //Controller.
    NGF::GameObject *controller;
    
    //Current CameraHandler.
    NGF::GameObject *currCameraHandler;

    //Currently shown MessageBox.
    NGF::GameObject *currMessageBox;

    //--- Global 'data' ----------------------------------------------------------------
    
    //Globally required data.
    bool paused;
    bool loadGame;
    unsigned int firstLevel; //World number of first level.
    Ogre::Real bonusTime; //Count down.

    //Settings.
    struct Settings
    {
        struct OgreSettings
        {
            Ogre::String pluginDirectory;
            Ogre::StringVector plugins;

            enum
            {
                OPENGL,
                DIRECT3D,
            } renderer;

            int winWidth, winHeight;
            bool winFullscreen;
            Ogre::String FSAA, vsync;
        } ogre;

        struct GraphicsSettings
        {
            bool lighting;
        } graphics;

        struct ControlSettings
        {
            Ogre::Real turningSensitivity;
            Ogre::Real upDownSensitivity;
            bool invertMouse;

            std::map<Ogre::String, OIS::KeyCode> keys;
        } controls;

        struct MiscSettings
        {
            bool fixCameraObstruction;
        } misc;
    } settings;

    //Records.
    struct Records
    {
        bool firstTime; //Whether first time (ie. no 'New Game' yet).
        unsigned int highestLevelIndex; //Index of highest level reached.

        //Records for each level.
        struct Record
        {
            unsigned int score;
            unsigned int losses;
            bool completed;
            bool checkpoint;

            friend class boost::serialization::access;
            template<class Archive>
            void serialize(Archive &ar, const unsigned int version)
            {
                ar & score;
                ar & losses;
                ar & completed;
                ar & checkpoint;
            }

            Record() //Default record values.
                : score(0),
                  losses(0),
                  completed(false),
                  checkpoint(false)
            {
            }
        };

        std::map<unsigned int, Record> recordMap;
    } records;

    //World to switch to. Required for delay in World-switching because GameObjects
    //can't destroy themselves.
#define PREV_WORLD -3
#define NEXT_WORLD -2
    int worldSwitch;
};

//'Procedures'.
void registerGameObjectTypes();
void addWorlds();
void initPythonBinds();
void initShadows();
void loadSettings();
void saveSettings();
void loadRecord();
void saveRecord();
void clearRecord();
void loseLevel();
void winLevel();

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
    MSG_GETPOSITION,          //Get position.
    MSG_GETTARGET,            //Get target (for CameraHandler).
    MSG_EXPLODE,              //Explode (for explosive objects).
    MSG_GETFRICTIONCOEFF,     //Get friction coefficient (for Ice).
    MSG_SETENABLED,           //Sets whether enabled (for SlidingBrush).
    MSG_SETFORWARD,           //Sets whether in 'forward' mode (for SlidingBrush).
    MSG_SETDIFFUSECOLOUR,     //Sets colour (for lights).
    MSG_SETPOSITION,          //Sets position.
    MSG_TELEPORT,             //Teleport (for Player).
    MSG_GETDIRECTION,         //Get direction (for Director).
    MSG_GETSPEED,             //Get speed (for Director).
    MSG_LEVELSTART,           //Level started (for Controller).
    MSG_LEVELSTOP,            //Level stopped (for Controller).
    MSG_CLICKED,              //Been clicked by ObjectClicker.
};

//Includes from the project used everywhere.
#include "Misc/DimensionManager.h"
#include "Misc/Console.h"
#include "Misc/Utils.h"
#include "Misc/Fader.h"
#include "Misc/MusicManager.h"
#include "Misc/VideoRecorder.h"
#include "Misc/KeyMap.h"
#include "Misc/OptionsDialog.h"
#include "Misc/ObjectClicker.h"
#include "Objects/Misc/AlarmGameObject.h"

#endif
