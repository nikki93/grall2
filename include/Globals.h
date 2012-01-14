/*
 * =====================================================================================
 *
 *       Filename:  Globals.h
 *
 *    Description:  Provides a 'Globals' class for easy global access of variables.
 *                  Also has some declarations for global functions and other global
 *                  stuff.
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

//Some compile-time settings
#ifdef USE_HOME
#define USER_PREFIX Ogre::String(getenv("HOME")) + "/.grall2/"
#else
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
#define USER_PREFIX Ogre::String(getenv("APPDATA")) + "/GraLL 2/"
#else
#define USER_PREFIX Ogre::String("../../usr/") + ""
#endif
#endif

#ifndef DATA_PREFIX
#define DATA_PREFIX "../../data/"
#endif

//Defines.
#define GlbVar Globals::getSingleton()
#define LOG(msg) Ogre::LogManager::getSingleton().logMessage(msg);
#define FORMAT(fmt, params) (boost::format(fmt) % params).str()
#define SET_PYTHON_SCRIPT() do                                                                 \
    {                                                                                          \
        Ogre::String script = mProperties.getValue("script", 0, "");                           \
        Ogre::String scriptObject = mProperties.getValue("scriptObject", 0, "");               \
        Ogre::String scriptFile = mProperties.getValue("scriptFile", 0, "");                   \
        if (script == "")                                                                      \
            if (scriptObject == "")                                                            \
                NGF::Python::PythonGameObject::setScriptFile(scriptFile, "General");           \
            else                                                                               \
                NGF::Python::PythonGameObject::setScriptCodeObject(scriptObject);              \
        else                                                                                   \
            NGF::Python::PythonGameObject::setScriptString(script);                            \
    } while (0)

//Forward declarations.
class DimensionManager;
class Console;
class Fader;
class MusicManager;
class VideoRecorder;
class KeyMap;
class OptionsDialog;
class ObjectClicker;
class HUD;
class GravityManager;

//The Globals class (actually a struct, but whatever).
struct Globals : public Ogre::Singleton<Globals>
{
    //--- Managers ---------------------------------------------------------------------
    
    //Ogre.
    Ogre::Root *ogreRoot;
    Ogre::SceneManager *ogreSmgr;
    Ogre::RenderWindow *ogreWindow;
    Ogre::Camera *ogreCamera;
    Ogre::SceneNode *camNode;

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
    HUD *hud;
    GravityManager *gravMgr;

    //--- Global 'objects' -------------------------------------------------------------

    //Player
    NGF::GameObject *player;

    //Controller.
    NGF::GameObject *controller;
    
    //Current CameraHandler.
    NGF::GameObject *currCameraHandler;

    //Currently shown MessageBox.
    NGF::GameObject *currMessageBox;

    //Currently shown SlideShow.
    NGF::GameObject *currSlideshow;

    //Preload overlay.
    Ogre::Entity *preloadEntity;

    //Sun.
    Ogre::Light *sun;

    //--- Global 'data' ----------------------------------------------------------------
    
    //Globally required data.
    bool paused;
    bool loadGame;
    unsigned int firstLevel; //World number of first level.
    Ogre::Real bonusTime; //Count down.

    std::vector<Ogre::String> ngfNames; //The NGF level script names ('official' levels).
    std::vector<Ogre::String> userNgfNames; //The NGF level script names ('user' levels).

    bool newLevel; //Whether this is a 'new level' (either highest, or user level).

    //Some persistent sounds.
    OgreAL::Sound *playerExplosionSound;
    OgreAL::Sound *bombExplosionSound;
    
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
            bool shadows;
            bool normalMapping;
            bool parallaxMapping;
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
void runPythonStartupScript();
void initShadows();
void loadSettings();
void saveSettings();
void loadRecord();
void saveRecord();
void clearRecord();
void loseLevel();
void winLevel(bool damp = true);

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
    MSG_WINLEVEL,             //Level won (for Controller).
    MSG_LOSELEVEL,            //Level lost (for Controller).
    MSG_CLICKED,              //Been clicked by ObjectClicker.
    MSG_GETPICKUPTYPE,        //Get Pickup type (for... Pickups).
    MSG_PICKEDUP,             //Pickup has been picked up (disables it).
    MSG_CAPTURECAMERAHANDLER, //Capture the CameraHandler.
    MSG_BULLETHIT,            //Bullet hit.
    MSG_ADDSLIDE,             //Add slide (for Slideshow).
    MSG_SETVISIBLE,           //Set visibility.
    MSG_ONEWAY,               //In a OneWay screen.
    MSG_MAGNET,               //In a magnetic field.
    MSG_GRAVITYCHANGE,        //Gravity changed.
    MSG_GETCRATEFORCE,        //Get 'crate-pushing' force (for Player).
    MSG_SHAKE,                //Camera shake (for CameraHandler).
};

//Flags for creating Physics bodies from Python.
struct PythonBodyFlags
{
    enum
    {
        CONVEX,
        TRIMESH,
        BOX,
        SPHERE,
        CYLINDERY,
        CYLINDERZ,
        CYLINDERX,
        MANUAL,

        FREE,
        STATIC,
        KINEMATIC,

        NO_CONTACT = 1<<16,
    };
};

//Includes from the project used everywhere.
#include "Objects/Misc/AlarmGameObject.h"
#include "Misc/DimensionManager.h"
#include "Misc/Console.h"
#include "Misc/Utils.h"
#include "Misc/Fader.h"
#include "Misc/MusicManager.h"
#include "Misc/VideoRecorder.h"
#include "Misc/KeyMap.h"
#include "Misc/OptionsDialog.h"
#include "Misc/ObjectClicker.h"
#include "Misc/HUD.h"
#include "Misc/GravityManager.h"

#endif
