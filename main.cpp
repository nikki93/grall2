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
    public:
        bool frameStarted(const Ogre::FrameEvent &evt)
        {
            //Temporary camera movement thing.
            OIS::MouseState ms = GlbVar.mouse->getMouseState();

            Ogre::Vector3 translate = Ogre::Vector3::ZERO;
            Ogre::Real speed = 15;

            speed *= GlbVar.keyboard->isKeyDown(OIS::KC_LSHIFT) ? 6 : 1; //'Sprint' mode.

            translate.z = (GlbVar.keyboard->isKeyDown(OIS::KC_DOWN) || GlbVar.keyboard->isKeyDown(OIS::KC_S)) 
                - (GlbVar.keyboard->isKeyDown(OIS::KC_UP) || GlbVar.keyboard->isKeyDown(OIS::KC_W));
            translate.x = (GlbVar.keyboard->isKeyDown(OIS::KC_RIGHT) || GlbVar.keyboard->isKeyDown(OIS::KC_D)) 
                - (GlbVar.keyboard->isKeyDown(OIS::KC_LEFT) || GlbVar.keyboard->isKeyDown(OIS::KC_A));
            translate *= speed;

            GlbVar.ogreCamera->yaw(Ogre::Degree(-ms.X.rel * 0.3));
            GlbVar.ogreCamera->pitch(Ogre::Degree(-ms.Y.rel * 0.3));
            GlbVar.ogreCamera->moveRelative(translate * evt.timeSinceLastFrame);

            //Physics update.
            GlbVar.phyWorld->stepSimulation(evt.timeSinceLastFrame, 7);
            GlbVar.phyWorld->debugDrawWorld();

            //NGF update.
            GlbVar.goMgr->tick(GlbVar.paused, evt);
            return GlbVar.woMgr->tick(evt);
        }

        //--- Send input events to MyGUI -----------------------------------------------
        bool keyPressed(const OIS::KeyEvent &arg)
        {
            switch (arg.key)
            {
                case OIS::KC_F7:
                    NGF::Serialisation::Serialiser::save("SaveFile");
                    break;

                case OIS::KC_F8:
                    GlbVar.goMgr->destroyAll();
                    NGF::Serialisation::Serialiser::load("SaveFile");
                    break;

                case OIS::KC_Z:
                    GlbVar.dimMgr->switchDimension();
                    break;
            }

            GlbVar.gui->injectKeyPress(arg);
            return true;
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
            //--- Globals --------------------------------------------------------------
            new Globals();

            //--- Ogre (Graphics) ------------------------------------------------------
            //Root.
            GlbVar.ogreRoot = new Ogre::Root("plugins.cfg", "config.cfg", "GraLL2.log");
            if (!GlbVar.ogreRoot->restoreConfig())
                if (!GlbVar.ogreRoot->showConfigDialog())
                {
                    Ogre::LogManager::getSingleton().logMessage("Configuration aborted, shutting down!");
                    delete GlbVar.ogreRoot;
                    return false;
                }

            //Window.
            GlbVar.ogreWindow = GlbVar.ogreRoot->initialise(true, "GraLL 2");

            //SceneManager.
            GlbVar.ogreSmgr = GlbVar.ogreRoot->createSceneManager(Ogre::ST_GENERIC);

            GlbVar.ogreSmgr->setAmbientLight(Ogre::ColourValue(0.6,0.6,0.6));
            Ogre::Light *light = GlbVar.ogreSmgr->createLight("mainLight");
            light->setType(Ogre::Light::LT_DIRECTIONAL);
            light->setDiffuseColour(Ogre::ColourValue(0.20,0.20,0.20));
            light->setSpecularColour(Ogre::ColourValue(0,0,0));
            light->setDirection(Ogre::Vector3(1,-2.5,1)); 

            //Camera, Viewport.
            GlbVar.ogreCamera = GlbVar.ogreSmgr->createCamera("mainCamera");
            Ogre::Viewport *viewport = GlbVar.ogreWindow->addViewport(GlbVar.ogreCamera);

            viewport->setDimensions(0,0,1,1);
            GlbVar.ogreCamera->setAspectRatio((float)viewport->getActualWidth() / (float) viewport->getActualHeight());
            GlbVar.ogreCamera->setFarClipDistance(1000.0);
            GlbVar.ogreCamera->setNearClipDistance(0.5);

            //Resources.
            Ogre::ResourceGroupManager &ogreRmgr = Ogre::ResourceGroupManager::getSingleton();

            ogreRmgr.addResourceLocation("../../data", "FileSystem", "General");
            ogreRmgr.addResourceLocation("../../data/GUI", "FileSystem", "General");
            ogreRmgr.addResourceLocation("../../data/ObjectMeshes", "FileSystem", "General");
            ogreRmgr.addResourceLocation("../../data/ObjectTextures", "FileSystem", "General");
            ogreRmgr.addResourceLocation("../../data/BrushMeshes", "FileSystem", "General");
            ogreRmgr.addResourceLocation("../../data/BrushTextures", "FileSystem", "General");
            ogreRmgr.addResourceLocation("../../data/Levels", "FileSystem", "General");
            //ogreRmgr.addResourceLocation("../../data/Shaders", "FileSystem", "General");

            ogreRmgr.initialiseAllResourceGroups();

            //--- OIS (Input) ----------------------------------------------------------
            OIS::ParamList params;
            size_t windowHnd = 0;

            GlbVar.ogreWindow->getCustomAttribute("WINDOW", &windowHnd);
            params.insert(std::make_pair(Ogre::String("WINDOW"), Ogre::StringConverter::toString(windowHnd)));
            //params.insert(std::make_pair(Ogre::String("x11_mouse_grab"), Ogre::String("false")));
            params.insert(std::make_pair(Ogre::String("x11_keyboard_grab"), Ogre::String("false")));

            mInputManager = OIS::InputManager::createInputSystem(params);
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

            //--- MyGUI (GUI) ----------------------------------------------------------
            GlbVar.gui = new MyGUI::Gui();
            GlbVar.gui->initialise(GlbVar.ogreWindow);

            //--- ~ (Sound) ------------------------------------------------------------

            //--- NGF (Game architecture framework) ------------------------------------
            //Usual stuff.
            GlbVar.goMgr = new NGF::GameObjectManager();
            GlbVar.woMgr = new NGF::WorldManager();
            GlbVar.lvlLoader = new NGF::Loading::Loader();

            //Python.
            Py_Initialize();
            new NGF::Python::PythonManager();

            return true;
        }

        void start()
        {
            //Not paused in the beginning.
            GlbVar.paused = false;

            //Create DimensionManager.
            GlbVar.dimMgr = new DimensionManager();

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

                //Exit if F12 pressed.
                if (GlbVar.keyboard->isKeyDown(OIS::KC_F12))
                    GlbVar.woMgr->shutdown();

                if(!GlbVar.ogreRoot->renderOneFrame())
                    break;
            }
        }

        void shutdown()
        {
            //DimensionManager.
            delete GlbVar.dimMgr;

            //NGF.
            Py_Finalize();
            delete NGF::Python::PythonManager::getSingletonPtr();
            delete GlbVar.lvlLoader;
            delete GlbVar.woMgr;
            delete GlbVar.goMgr;

            //Physics.
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
