/*
 * =====================================================================================
 *
 *       Filename:  main.cpp
 *
 *    Description:  The main source file for GraLL 2.
 *
 *        Created:  04/26/2009 07:35:28 PM
 *       Compiler:  gcc
 *
 *         Author:  Nikhilesh (nikki)
 *
 * =====================================================================================
 */

#include "Globals.h"

#include "Objects/Main/Controller.h"

#include "boost/format.hpp"

#include "BulletCollision/CollisionDispatch/btGhostObject.h"

template<> Globals* Ogre::Singleton<Globals>::ms_Singleton = 0;

//--------------------------------------------------------------------------------------
class MaterialListener : 
    public Ogre::MaterialManager::Listener
{
    protected:
        Ogre::MaterialPtr mBlackMat;
    public:
        MaterialListener()
        {
            mBlackMat = Ogre::MaterialManager::getSingleton().create("mBlack", "Internal");
            mBlackMat->getTechnique(0)->getPass(0)->setDiffuse(0,0,0,0);
            mBlackMat->getTechnique(0)->getPass(0)->setSpecular(0,0,0,0);
            mBlackMat->getTechnique(0)->getPass(0)->setAmbient(0,0,0);
            mBlackMat->getTechnique(0)->getPass(0)->setSelfIllumination(0,0,0);
        }

        Ogre::Technique *handleSchemeNotFound(unsigned short, const Ogre::String& schemeName, 
                Ogre::Material*, unsigned short, const Ogre::Renderable*)
        {
            return mBlackMat->getTechnique(0);
        }
}; 

//--------------------------------------------------------------------------------------
class GameListener :
    public Ogre::FrameListener,
    public OIS::KeyListener,
    public OIS::MouseListener
{
    protected:
        static OIS::KeyCode mCurrKey; //Stores keycode to broadcast to GameObjects.

    public:
        bool frameStarted(const Ogre::FrameEvent &evt)
        {
            //GUI update.
            GlbVar.gui->injectFrameEntered(evt.timeSinceLastFrame);

            //Physics update.
            if (!GlbVar.paused)
                GlbVar.phyWorld->stepSimulation(evt.timeSinceLastFrame, 7);
            GlbVar.phyWorld->debugDrawWorld();

            //Shows debug if F10 key down.
            GlbVar.phyDebug->setDebugMode(GlbVar.keyboard->isKeyDown(OIS::KC_F10));
            GlbVar.phyDebug->step();

            //Update helpers.
            GlbVar.dimMgr->tick();
            GlbVar.fader->tick(evt);
            GlbVar.musicMgr->tick(evt);
            GlbVar.videoRec->tick(evt);
            GlbVar.optionsDialog->tick();

            //NGF update.
            GlbVar.goMgr->tick(GlbVar.paused, evt);
            return GlbVar.woMgr->tick(evt);
        }

        //--- Send keypress events to GameObjects, and all events to MyGUI -------------
        bool keyPressed(const OIS::KeyEvent &arg)
        {
            mCurrKey = arg.key;

            //Tell helpers.
            GlbVar.console->keyPressed(mCurrKey);
            GlbVar.optionsDialog->keyPressed(mCurrKey);

            //Tell MyGUI.
            GlbVar.gui->injectKeyPress(arg);

            //Tell all GameObjects.
            if (!GlbVar.console->isVisible())
                GlbVar.goMgr->forEachGameObject(GameListener::sendKeyPressMessage);

            //Some debug keys. TODO: Remove these in final version.
            //|F1, F2: Level switch | F3, F4: Console | F5, F6: Video | F7, F8: Test save/load | F9: Screenshot |
            switch (mCurrKey)
            {
                case OIS::KC_F1:
                    GlbVar.woMgr->previousWorld();
                    break;
                case OIS::KC_F2:
                    GlbVar.woMgr->nextWorld();
                    break;

                case OIS::KC_F5:
                    GlbVar.videoRec->startRecording(0.1);
                    break;
                case OIS::KC_F6:
                    GlbVar.videoRec->stopRecording();
                    break;

                case OIS::KC_F7:
                    serialise("TestSave");
                    break;
                case OIS::KC_F8:
                    clearLevel();
                    deserialise("TestSave");
                    break;

                case OIS::KC_F9:
                    //highResScreenshot(GlbVar.ogreWindow, GlbVar.ogreCamera, 3, "HiResScreenshot", ".jpg", true);
                    GlbVar.ogreWindow->writeContentsToFile("Screenshot.png");
                    break;

                case OIS::KC_O:
                    GlbVar.optionsDialog->setVisible(true);
                    break;
            }

            return true;
        }
        static void sendKeyPressMessage(NGF::GameObject *obj)
        {
            GlbVar.goMgr->sendMessage(obj, NGF_MESSAGE(MSG_KEYPRESSED, mCurrKey));
        }

        bool keyReleased(const OIS::KeyEvent &arg)
        {
            GlbVar.gui->injectKeyRelease(arg);
            return true;
        }

        bool mouseMoved(const OIS::MouseEvent &arg)
        {
            GlbVar.gui->injectMouseMove(arg);
            return true;
        }
        bool mousePressed(const OIS::MouseEvent &arg, OIS::MouseButtonID id)
        {
            GlbVar.gui->injectMousePress(arg, id);
            return true;
        }
        bool mouseReleased(const OIS::MouseEvent &arg, OIS::MouseButtonID id)
        {
            GlbVar.gui->injectMouseRelease(arg, id);
            return true;
        }
};
OIS::KeyCode GameListener::mCurrKey = OIS::KC_UNASSIGNED;

//--------------------------------------------------------------------------------------
class Game
{
    protected:
        OIS::InputManager *mInputManager;

        GameListener *mGameListener;
        MaterialListener *mMaterialListener;

        btAxisSweep3 *mBroadphase;
        btDefaultCollisionConfiguration *mCollisionConfig;
        btCollisionDispatcher *mDispatcher;
        btSequentialImpulseConstraintSolver *mSolver;

    public:
        bool init()
        {
            //--- Globals, settings ----------------------------------------------------
            new Globals();
            GlbVar.keyMap = new KeyMap();
            loadSettings();

            //--- Ogre (Graphics) ------------------------------------------------------
            //Root.
            GlbVar.ogreRoot = new Ogre::Root("", "", "GraLL2.log");

            /*
            if (!GlbVar.ogreRoot->restoreConfig())
                if (!GlbVar.ogreRoot->showConfigDialog())
                {
                    Ogre::LogManager::getSingleton().logMessage("Configuration aborted, shutting down!");
                    delete GlbVar.ogreRoot;
                    return false;
                }
            */

            //Plugins.
            switch (GlbVar.settings.ogre.renderer)
            {
                case Globals::Settings::OgreSettings::DIRECT3D:
                    GlbVar.ogreRoot->loadPlugin(GlbVar.settings.ogre.pluginDirectory + "/RenderSystem_Direct3D9");
                    break;

                case Globals::Settings::OgreSettings::OPENGL:
                    GlbVar.ogreRoot->loadPlugin(GlbVar.settings.ogre.pluginDirectory + "/RenderSystem_GL");
                    break;
            }

            for (Ogre::StringVector::iterator iter = GlbVar.settings.ogre.plugins.begin();
                    iter != GlbVar.settings.ogre.plugins.end(); ++iter)
            {
                GlbVar.ogreRoot->loadPlugin(GlbVar.settings.ogre.pluginDirectory + "/" + (*iter));
            }

            //Resources.
            Ogre::ResourceGroupManager &ogreRmgr = Ogre::ResourceGroupManager::getSingleton();

            ogreRmgr.addResourceLocation(".", "FileSystem", "General");
            ogreRmgr.addResourceLocation("../../data", "FileSystem", "General");
            ogreRmgr.addResourceLocation("../../data/GUI", "FileSystem", "General");
            ogreRmgr.addResourceLocation("../../data/GUI/Layouts", "FileSystem", "General");
            ogreRmgr.addResourceLocation("../../data/Levels", "FileSystem", "General");
            ogreRmgr.addResourceLocation("../../data/Sounds", "FileSystem", "General");

            ogreRmgr.addResourceLocation("../../data/ObjectMeshes", "FileSystem", "General");
            ogreRmgr.addResourceLocation("../../data/ObjectTextures", "FileSystem", "General");

            ogreRmgr.addResourceLocation("../../data/BrushMeshes", "FileSystem", "General");
            ogreRmgr.addResourceLocation("../../data/BrushTextures", "FileSystem", "General");
            ogreRmgr.addResourceLocation("../../data/BrushTextures/Concrete", "FileSystem", "General");
            ogreRmgr.addResourceLocation("../../data/BrushTextures/Metal", "FileSystem", "General");
            ogreRmgr.addResourceLocation("../../data/BrushTextures/Other", "FileSystem", "General");
            ogreRmgr.addResourceLocation("../../data/BrushTextures/Special", "FileSystem", "General");
            ogreRmgr.addResourceLocation("../../data/BrushTextures/Tile", "FileSystem", "General");

            ogreRmgr.addResourceLocation("../../data/Shaders", "FileSystem", "General");
            ogreRmgr.addResourceLocation("../../data/Shaders/Base", "FileSystem", "General");
            ogreRmgr.addResourceLocation("../../data/Shaders/Shadows", "FileSystem", "General");
            ogreRmgr.addResourceLocation("../../data/Compositors", "FileSystem", "General");
            ogreRmgr.addResourceLocation("../../data/ParticleFX", "FileSystem", "General");

            //Renderer. Just choose the first available one, we just load only one plugin (either
            //Direct3D or OpenGL) anyway.
            const Ogre::RenderSystemList &renderers = GlbVar.ogreRoot->getAvailableRenderers();
            Ogre::RenderSystem *renderer = *(renderers.begin());
            GlbVar.ogreRoot->setRenderSystem(renderer);

            //Window.
            GlbVar.ogreRoot->initialise(false);

            Ogre::NameValuePairList winParams;
            winParams["FSAA"] = GlbVar.settings.ogre.FSAA;
            winParams["vsync"] = GlbVar.settings.ogre.vsync;

            GlbVar.ogreWindow = GlbVar.ogreRoot->createRenderWindow(
                    "GraLL 2",
                    GlbVar.settings.ogre.winWidth, 
                    GlbVar.settings.ogre.winHeight,
                    GlbVar.settings.ogre.winFullscreen,
                    &winParams
                    );
            GlbVar.ogreWindow->setActive(true);
            GlbVar.ogreWindow->setAutoUpdated(true);

            //SceneManager, main lights.
            GlbVar.ogreSmgr = GlbVar.ogreRoot->createSceneManager(Ogre::ST_GENERIC);
            GlbVar.ogreSmgr->setAmbientLight(Ogre::ColourValue(0.49,0.49,0.49));

            /*
            GlbVar.ogreSmgr->setShadowTechnique(Ogre::SHADOWTYPE_STENCIL_ADDITIVE);
            GlbVar.ogreSmgr->setShadowColour(Ogre::ColourValue(0.7,0.7,0.7));
            */
            
            /*
            Ogre::Light *light = GlbVar.ogreSmgr->createLight("mainLight");
            light->setType(Ogre::Light::LT_DIRECTIONAL);
            light->setDiffuseColour(Ogre::ColourValue(0.15,0.15,0.15));
            light->setSpecularColour(Ogre::ColourValue(0,0,0));
            light->setDirection(Ogre::Vector3(1,-2.5,1)); 
            */

            //Camera, Viewport.
            GlbVar.ogreCamera = GlbVar.ogreSmgr->createCamera("mainCamera");
            Ogre::Viewport *viewport = GlbVar.ogreWindow->addViewport(GlbVar.ogreCamera);

            viewport->setDimensions(0,0,1,1);
            GlbVar.ogreCamera->setAspectRatio((float)viewport->getActualWidth() / (float) viewport->getActualHeight());
            GlbVar.ogreCamera->setFarClipDistance(1000.0);
            GlbVar.ogreCamera->setNearClipDistance(0.5);

            //--- OIS (Input) ----------------------------------------------------------
            OIS::ParamList inputParams;
            size_t windowHnd = 0;

            GlbVar.ogreWindow->getCustomAttribute("WINDOW", &windowHnd);
            inputParams.insert(std::make_pair(Ogre::String("WINDOW"), Ogre::StringConverter::toString(windowHnd)));
            //inputParams.insert(std::make_pair(Ogre::String("x11_mouse_grab"), Ogre::String("false")));
            inputParams.insert(std::make_pair(Ogre::String("x11_keyboard_grab"), Ogre::String("false")));

            mInputManager = OIS::InputManager::createInputSystem(inputParams);
            GlbVar.keyboard = static_cast<OIS::Keyboard*>(mInputManager->createInputObject(OIS::OISKeyboard, true));
            GlbVar.mouse = static_cast<OIS::Mouse*>(mInputManager->createInputObject(OIS::OISMouse, true));

            unsigned int width, height, depth; int top, left;
            GlbVar.ogreWindow->getMetrics(width, height, depth, left, top);
            const OIS::MouseState &ms = GlbVar.mouse->getMouseState();
            ms.width = width;
            ms.height = height;

            //--- Listeners ------------------------------------------------------------
            mGameListener = new GameListener();
            GlbVar.ogreRoot->addFrameListener(mGameListener);
            GlbVar.keyboard->setEventCallback(mGameListener);
            GlbVar.mouse->setEventCallback(mGameListener);

            mMaterialListener = new MaterialListener();
            Ogre::MaterialManager::getSingleton().addListener(mMaterialListener);

            //--- Bullet (Physics) -----------------------------------------------------
            mBroadphase = new btAxisSweep3(btVector3(-10000,-10000,-10000), btVector3(10000,10000,10000), 1024);
            mBroadphase->getOverlappingPairCache()->setInternalGhostPairCallback(new btGhostPairCallback());
            mCollisionConfig = new btDefaultCollisionConfiguration();
            mDispatcher = new btCollisionDispatcher(mCollisionConfig);
            mSolver = new btSequentialImpulseConstraintSolver();

            GlbVar.phyWorld = new btDiscreteDynamicsWorld(mDispatcher, mBroadphase, mSolver, mCollisionConfig);

            GlbVar.phyDebug = new BtOgre::DebugDrawer(GlbVar.ogreSmgr->getRootSceneNode(), GlbVar.phyWorld);
            GlbVar.phyWorld->setDebugDrawer(GlbVar.phyDebug);

            //--- MyGUI (GUI) ----------------------------------------------------------
            GlbVar.gui = new MyGUI::Gui();
            GlbVar.gui->initialise(GlbVar.ogreWindow);

            //--- OgreAL (Sound) -------------------------------------------------------
            GlbVar.soundMgr = new OgreAL::SoundManager();

            //--- NGF (Game architecture framework) ------------------------------------
            //Usual stuff.
            GlbVar.goMgr = new NGF::GameObjectManager();
            GlbVar.woMgr = new NGF::WorldManager();
            GlbVar.lvlLoader = new NGF::Loading::Loader();

            //Python, GraLL2 python bindings, python console.
            Py_Initialize();
            GlbVar.console = new Console();
            new NGF::Python::PythonManager(fastdelegate::MakeDelegate(GlbVar.console, &Console::print));
            initPythonBinds();

            //--- Init resources and other stuff ---------------------------------------
            //Initialise the ResourceGroups.
            ogreRmgr.initialiseAllResourceGroups();

            //Shadows.
            //initShadows();
            
            //Compositor chain.
            Ogre::CompositorManager::getSingleton().addCompositor(viewport, "Compositor/Glow");
            Ogre::CompositorManager::getSingleton().addCompositor(viewport, "Compositor/Dimension2");
            Ogre::CompositorManager::getSingleton().setCompositorEnabled(viewport, "Compositor/Glow", true);
            //MyGUI::StaticImagePtr img = GlbVar.gui->createWidget<MyGUI::StaticImage>("StaticImage", 100,100,200,200, MyGUI::Align::Default, "Main");
            //img->setImageTexture(GlbVar.ogreSmgr->getShadowTexture(0)->getName());
            //img->setVisible(true);

            return true;
        }

        void start()
        {
            //Not paused in the beginning.
            GlbVar.paused = false;

            //Create helpers.
            GlbVar.dimMgr = new DimensionManager();
            GlbVar.fader = new Fader();
            GlbVar.musicMgr = new MusicManager();
            GlbVar.videoRec = new VideoRecorder();
            GlbVar.optionsDialog = new OptionsDialog();

            //The persistent Controller GameObject.
            GlbVar.controller = GlbVar.goMgr->createObject<Controller>(Ogre::Vector3::ZERO, Ogre::Quaternion::ZERO);

            //Initialise other Global variables.
            GlbVar.currCameraHandler = 0;
            GlbVar.currMessageBox = 0;
            GlbVar.worldSwitch = -1;
            GlbVar.loadGame = true;

            //Add Worlds, register GameObjects.
            addWorlds();
            registerGameObjectTypes();

            //Load user record (highest level, times, scores).
            loadRecord();

            //Start running the Worlds.
            GlbVar.woMgr->start(0);
        }

        void loop()
        {
            while (true)
            {
                //Window message pumping, events etc.
                Ogre::WindowEventUtilities::messagePump();

                //Update input.
                GlbVar.keyboard->capture();
                GlbVar.mouse->capture();

                //If we need to switch Worlds, do so, but not again.
                if (GlbVar.worldSwitch != -1)
                {
                    //If 'special' index, go forward or backward, else just jump.
                    if (GlbVar.worldSwitch < 0)
                        if (GlbVar.worldSwitch == PREV_WORLD)
                            GlbVar.woMgr->previousWorld();
                        else
                            GlbVar.woMgr->nextWorld();
                    else
                        GlbVar.woMgr->gotoWorld(GlbVar.worldSwitch);
                    GlbVar.worldSwitch = -1;
                }

                //Exit if F12 pressed.
                if (GlbVar.keyboard->isKeyDown(OIS::KC_F12))
                    GlbVar.woMgr->shutdown();

                //Update Ogre.
                if(!GlbVar.ogreRoot->renderOneFrame())
                    break;
            }
        }

        void shutdown()
        {
            //Controller can't truly live forever. :P
            if (GlbVar.controller)
                GlbVar.goMgr->destroyObject(GlbVar.controller->getID());

            //Save settings, user record.
            saveRecord();
            saveSettings();

            //Helpers.
            delete GlbVar.optionsDialog;
            delete GlbVar.videoRec;
            delete GlbVar.musicMgr;
            delete GlbVar.fader;
            delete GlbVar.dimMgr;
            delete GlbVar.console;
            delete GlbVar.keyMap;

            //NGF.
            Py_Finalize();
            delete NGF::Python::PythonManager::getSingletonPtr();
            delete GlbVar.lvlLoader;
            delete GlbVar.woMgr;
            delete GlbVar.goMgr;

            //Sound.
            delete GlbVar.soundMgr;

            //Physics.
            delete GlbVar.phyDebug;
            delete GlbVar.phyWorld;
            delete mSolver;
            delete mDispatcher;
            delete mCollisionConfig;
            delete mBroadphase;

            //Listeners.
            delete mMaterialListener;
            delete mGameListener;

            //Input.
            mInputManager->destroyInputObject(GlbVar.mouse);
            mInputManager->destroyInputObject(GlbVar.keyboard);
            OIS::InputManager::destroyInputSystem(mInputManager);

            //Graphics.
            delete GlbVar.ogreRoot;

            //Globals.
            delete Globals::getSingletonPtr();
        }
};

//--------------------------------------------------------------------------------------
int main(int argc, char *argv[])
{
    Game game;
    try
    {
        //Init.
        if(!(game.init()))
            return 0;   
        Ogre::LogManager::getSingleton().logMessage("*********** Systems Intialised *************\n");
        game.start();

        //Loop.
        Ogre::LogManager::getSingleton().logMessage("*********** Entering Mainloop **************\n");
        game.loop();
        Ogre::LogManager::getSingleton().logMessage("*********** Mainloop Exited ****************\n");

        //Shutdown.
        Ogre::LogManager::getSingleton().logMessage("*********** Shutting Down Systems **********\n");
        game.shutdown();
    }
    catch(OIS::Exception &e)
    {
        Ogre::StringUtil::StrStreamType desc;

        desc << 
            "An exception has been thrown!\n"
            "\n"
            "-----------------------------------\n"
            "Details:\n"
            "-----------------------------------\n"
            "Error #: " << e.eType
            << "\nDescription: " << e.eText 
            << ". "
            << "\nFile: " << e.eFile
            << "\nLine: " << e.eLine;

        std::cerr << desc.str();
        Ogre::LogManager::getSingleton().logMessage(desc.str());
    }
    catch (std::exception &e)
    {
        Ogre::StringUtil::StrStreamType desc;

        desc << 
            "An exception has been thrown!\n"
            "\n"
            "-----------------------------------\n"
            "Details:\n"
            "-----------------------------------\n"
            << "\nDescription: " << e.what();

        std::cerr << desc.str();
        Ogre::LogManager::getSingleton().logMessage(desc.str());
    }
    catch (py::error_already_set)
    {
        PyErr_Print();
    }
    catch (...)
    {
        std::cerr << "Uknown exception thrown!";
    }

    return 0;
}
