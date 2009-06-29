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

#include "boost/format.hpp"

template<> Globals* Ogre::Singleton<Globals>::ms_Singleton = 0;

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
            //Physics update.
            if (!GlbVar.paused)
                GlbVar.phyWorld->stepSimulation(evt.timeSinceLastFrame, 7);
            GlbVar.phyWorld->debugDrawWorld();

            //Shows debug if F3 key down.
            GlbVar.phyDebug->setDebugMode(GlbVar.keyboard->isKeyDown(OIS::KC_F3));
            GlbVar.phyDebug->step();

            //Update the Fader.
            GlbVar.fader->tick(evt);

            //NGF update.
            GlbVar.goMgr->tick(GlbVar.paused, evt);
            return GlbVar.woMgr->tick(evt);
        }

        //--- Send keypress events to GameObjects, and all events to MyGUI -------------
        bool keyPressed(const OIS::KeyEvent &arg)
        {
            mCurrKey = arg.key;

            //Tell console.
            GlbVar.console->keyPressed(mCurrKey);

            //Tell MyGUI.
            GlbVar.gui->injectKeyPress(arg);

            //Tell all GameObjects.
            if (!GlbVar.console->isVisible())
                GlbVar.goMgr->forEachGameObject(GameListener::sendKeyPressMessage);

            //Some stuff we have to do. TODO: Shouldn't be needed in final version.
            switch (mCurrKey)
            {
                case OIS::KC_F7:
                    NGF::Serialisation::Serialiser::save(USER_PREFIX + "Saves/TestSave.sav");
                    break;
                case OIS::KC_F8:
                    clearLevel();
                    NGF::Serialisation::Serialiser::load(USER_PREFIX + "Saves/TestSave.sav");
                    break;

                case OIS::KC_N:
                    GlbVar.woMgr->nextWorld();
                    break;
                case OIS::KC_P:
                    GlbVar.woMgr->previousWorld();
                    break;

                case OIS::KC_F9:
                    highResScreenshot(GlbVar.ogreWindow, GlbVar.ogreCamera, 3, "HiResScreenshot", ".jpg", true);
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

        btAxisSweep3 *mBroadphase;
        btDefaultCollisionConfiguration *mCollisionConfig;
        btCollisionDispatcher *mDispatcher;
        btSequentialImpulseConstraintSolver *mSolver;

    public:
        bool init()
        {
            //--- Globals, settings ----------------------------------------------------
            new Globals();
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
                    GlbVar.ogreRoot->loadPlugin(GlbVar.settings.ogre.pluginDirectory + "RenderSystem_Direct3D9");
                    break;

                case Globals::Settings::OgreSettings::OPENGL:
                    GlbVar.ogreRoot->loadPlugin(GlbVar.settings.ogre.pluginDirectory + "RenderSystem_GL");
                    break;
            }

            for (Ogre::StringVector::iterator iter = GlbVar.settings.ogre.plugins.begin();
                    iter != GlbVar.settings.ogre.plugins.end(); ++iter)
            {
                GlbVar.ogreRoot->loadPlugin(GlbVar.settings.ogre.pluginDirectory + (*iter));
            }

            //Renderer.
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

            //Resources.
            Ogre::ResourceGroupManager &ogreRmgr = Ogre::ResourceGroupManager::getSingleton();

            ogreRmgr.addResourceLocation(".", "FileSystem", "General");
            ogreRmgr.addResourceLocation("../../data", "FileSystem", "General");
            ogreRmgr.addResourceLocation("../../data/GUI", "FileSystem", "General");
            ogreRmgr.addResourceLocation("../../data/Levels", "FileSystem", "General");

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

            //--- GameListener ---------------------------------------------------------
            mGameListener = new GameListener();

            GlbVar.ogreRoot->addFrameListener(mGameListener);
            GlbVar.keyboard->setEventCallback(mGameListener);
            GlbVar.mouse->setEventCallback(mGameListener);

            //--- Bullet (Physics) -----------------------------------------------------
            mBroadphase = new btAxisSweep3(btVector3(-10000,-10000,-10000), btVector3(10000,10000,10000), 1024);
            mCollisionConfig = new btDefaultCollisionConfiguration();
            mDispatcher = new btCollisionDispatcher(mCollisionConfig);
            mSolver = new btSequentialImpulseConstraintSolver();

            GlbVar.phyWorld = new btDiscreteDynamicsWorld(mDispatcher, mBroadphase, mSolver, mCollisionConfig);

            ogreRmgr.createResourceGroup("BtOgre");
            GlbVar.phyDebug = new BtOgre::DebugDrawer(GlbVar.ogreSmgr->getRootSceneNode(), GlbVar.phyWorld);
            GlbVar.phyWorld->setDebugDrawer(GlbVar.phyDebug);

            //--- MyGUI (GUI) ----------------------------------------------------------
            GlbVar.gui = new MyGUI::Gui();
            GlbVar.gui->initialise(GlbVar.ogreWindow);

            //--- ~ (Sound) ------------------------------------------------------------

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
            ogreRmgr.initialiseAllResourceGroups();
            initShadows();
            //MyGUI::StaticImagePtr img = GlbVar.gui->createWidget<MyGUI::StaticImage>("StaticImage", 100,100,200,200, MyGUI::Align::Default, "Main");
            //img->setImageTexture(GlbVar.ogreSmgr->getShadowTexture(0)->getName());
            //img->setVisible(true);

            return true;
        }

        void start()
        {
            //Not paused in the beginning.
            GlbVar.paused = false;

            //Create DimensionManager.
            GlbVar.dimMgr = new DimensionManager();

            //Fader.
            GlbVar.fader = new Fader();

            //Initialise other Global variables.
            GlbVar.currCameraHandler = 0;
            GlbVar.currMessageBox = 0;
            GlbVar.worldSwitch = -1;
            GlbVar.loadGame = true;
            GlbVar.levelName = "";

            //Load settings. Set to defualt first, then load.
            /*
            GlbVar.settings.controls.turningSensitivity = 0.2;
            GlbVar.settings.controls.upDownSensitivity = 1;
            GlbVar.settings.controls.invertMouse = true;

            GlbVar.settings.controls.forward = OIS::KC_W;
            GlbVar.settings.controls.backward = OIS::KC_S;
            GlbVar.settings.controls.left = OIS::KC_A;
            GlbVar.settings.controls.right = OIS::KC_D;

            GlbVar.settings.controls.dimensionSwitch = OIS::KC_SPACE;
            GlbVar.settings.controls.selfDestruct = OIS::KC_F10;

            GlbVar.settings.controls.peepLeft = OIS::KC_Q;
            GlbVar.settings.controls.peepRight = OIS::KC_E;
            */

            //Add Worlds, register GameObjects.
            addWorlds();
            registerGameObjectTypes();

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

                //Update DimensionManager.
                GlbVar.dimMgr->tick();

                //If we need to switch Worlds, do so, but not again.
                if (GlbVar.worldSwitch != -1)
                {
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
            //Save settings.
            saveSettings();

            //DimensionManager.
            delete GlbVar.dimMgr;

            //Console.
            delete GlbVar.console;

            //NGF.
            Py_Finalize();
            delete NGF::Python::PythonManager::getSingletonPtr();
            delete GlbVar.lvlLoader;
            delete GlbVar.woMgr;
            delete GlbVar.goMgr;

            //Physics.
            delete GlbVar.phyDebug;
            delete GlbVar.phyWorld;
            delete mSolver;
            delete mDispatcher;
            delete mCollisionConfig;
            delete mBroadphase;

            //GameListener.
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
