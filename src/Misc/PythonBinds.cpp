/*
 * =====================================================================================
 *
 *       Filename:  PythonBinds.cpp
 *
 *    Description:  The GraLL2-Python library.
 *
 *        Created:  06/11/2009 09:54:33 PM
 *       Compiler:  gcc
 *
 *         Author:  Nikhilesh (nikki)
 *
 * =====================================================================================
 */

#include "Globals.h"
#include "Objects/Misc/Slideshow.h"

//--- Wrapper functions ---------------------------------------------------------

//Show message box.
NGF::Python::PythonObjectConnectorPtr py_showMessage(Ogre::String message, Ogre::Real time)
{
    NGF::GameObject *msg = Util::showMessage(message, time);
    NGF::Python::PythonGameObject *pyMsg = dynamic_cast<NGF::Python::PythonGameObject*>(msg);
    return pyMsg->getConnector();
}

//World switching.
void py_nextWorld() 
{ 
    Util::nextWorld();
}
void py_previousWorld() 
{ 
    Util::previousWorld(); 
}
void py_gotoWorld(int index) 
{ 
    Util::gotoWorld(index); 
}

//NGF level load/clear.
void py_loadNgf(Ogre::String name)
{
    Util::loadNgf(name);
}
void py_clearLevel()
{
    Util::clearLevel();
}

//Serialise/deserialise.
void py_serialise(Ogre::String name)
{
    Util::serialise(name);
}
void py_deserialise(Ogre::String name)
{
    Util::deserialise(name);
}

//Dimension stuff (if you want to switch dimensions, best you do it through the 
//Player object).
void py_switchDimension() 
{ 
    GlbVar.dimMgr->switchDimension(); 
}
void py_setDimension(int dimension) 
{ 
    GlbVar.dimMgr->setDimension(dimension); 
}

//Fades.
void py_fadeInScreen(const Ogre::ColourValue &colour, Ogre::Real time) 
{ 
    GlbVar.fader->fadeIn(colour, time); 
}
void py_fadeOutScreen(const Ogre::ColourValue &colour, Ogre::Real time) 
{ 
    GlbVar.fader->fadeOut(colour, time); 
}
void py_fadeInOutScreen(const Ogre::ColourValue &colour, Ogre::Real in, Ogre::Real pause, Ogre::Real out) 
{ 
    GlbVar.fader->fadeInOut(colour, in, pause, out); 
}

//Level stuff.
bool py_saveExists(unsigned int levelNum)
{
    return Util::getRecordFromLevelNum(levelNum).checkpoint;
}
void py_removeSave(unsigned int levelNum)
{
    Util::getRecordFromLevelNum(levelNum).checkpoint = false;
}
unsigned int py_getCurrentLevelNum()
{
    return Util::worldNumToLevelNum(GlbVar.woMgr->getCurrentWorldIndex());
}
void py_winLevel()
{
    winLevel();
}
void py_loseLevel()
{
    loseLevel();
}

//Music stuff.
void py_playMusic(Ogre::String music)
{
    GlbVar.musicMgr->playMusic(music);
}
void py_crossFadeMusic(Ogre::String music, Ogre::Real time)
{
    GlbVar.musicMgr->crossFade(music, time);
}
void py_setMusicGain(Ogre::Real gain)
{
    GlbVar.musicMgr->setGain(gain);
}
Ogre::Real py_getMusicGain()
{
    return GlbVar.musicMgr->getGain();
}

//Video recording stuff.
void py_startRecordingVideo(Ogre::Real period)
{
    GlbVar.videoRec->startRecording(period);
}
void py_stopRecordingVideo()
{
    GlbVar.videoRec->stopRecording();
}

//Control setting.
void py_setControl(Ogre::String key, Ogre::String value)
{
    GlbVar.settings.controls.keys[key] = GlbVar.keyMap->stringToKey(value, OIS::KC_UNASSIGNED);
}
Ogre::String py_getControl(Ogre::String key)
{
    return GlbVar.keyMap->keyToString(GlbVar.settings.controls.keys[key]);
}

//PythonObject clicking.
void py_clickObject()
{
    GlbVar.objectClicker->click();
}
NGF::Python::PythonObjectConnectorPtr py_getClickedObject()
{
    NGF::GameObject *gobj = GlbVar.objectClicker->getClickedObject();
    NGF::Python::PythonGameObject *pobj = dynamic_cast<NGF::Python::PythonGameObject*>(gobj);

    return pobj ? (pobj->getConnector()) : NGF::Python::PythonObjectConnectorPtr();
}

//Screenshots.
void py_screenshot(bool highRes, Ogre::String filename, Ogre::String extension)
{
    if (highRes)
        Util::highResScreenshot(GlbVar.ogreWindow, GlbVar.ogreCamera, 3, filename, extension, true);
    else
        Util::screenshot(filename, extension);
}

//Bonus time.
void py_setBonusTime(Ogre::Real time)
{
    GlbVar.bonusTime = 0;
}
Ogre::Real py_getBonusTime()
{
    return GlbVar.bonusTime;
}

void py_startSlideshow(int width, int height, py::list slides)
{
    if (!GlbVar.newLevel)
        return;

    NGF::GameObject *obj = GlbVar.goMgr->createObject<Slideshow>(Ogre::Vector3::ZERO, Ogre::Quaternion::IDENTITY, NGF::PropertyList::create
            ("width", Ogre::StringConverter::toString(width))
            ("height", Ogre::StringConverter::toString(height))
            );

    try
    {
        while(1)
        {
            py::tuple t = py::extract<py::tuple>(slides.pop(0));
            Ogre::Real time = py::extract<Ogre::Real>(t[0]);
            Ogre::String image = py::extract<Ogre::String>(t[1]);
            GlbVar.goMgr->sendMessage(obj, NGF_MESSAGE(MSG_ADDSLIDE, time, image));
        }
    }
    catch (...)
    {
        return;
    }
}


//--- The module ----------------------------------------------------------------

BOOST_PYTHON_MODULE(GraLL2)
{
    //Docstring settings.
    py::docstring_options doc_options(true, true, false);

    //Enums.
    py::enum_<int>("Dimensions")
        .value("Dim_1", DimensionManager::DIM_1)
        .value("Dim_2", DimensionManager::DIM_2)
        .value("Dim_Both", DimensionManager::DIM_BOTH)
        ;

    py::enum_<int>("CollisionShape")
        .value("Convex", PythonBodyFlags::CONVEX)
        .value("Trimesh", PythonBodyFlags::TRIMESH)
        .value("Box", PythonBodyFlags::BOX)
        .value("Sphere", PythonBodyFlags::SPHERE)
        .value("CylinderY", PythonBodyFlags::CYLINDERY)
        .value("CylinderZ", PythonBodyFlags::CYLINDERZ)
        .value("CylinderX", PythonBodyFlags::CYLINDERX)
        .value("Manual", PythonBodyFlags::MANUAL)
        ;

    py::enum_<int>("BodyType")
        .value("Free", PythonBodyFlags::FREE)
        .value("Static", PythonBodyFlags::STATIC)
        .value("Kinematic", PythonBodyFlags::KINEMATIC)
        .value("No_Contact", PythonBodyFlags::NO_CONTACT)
        ;

    py::enum_<int>("CollisionFlags")
        .value("None", 0)

        .value("Player", DimensionManager::PLAYER)
        .value("Static", DimensionManager::STATIC)
        .value("Movingbomb", DimensionManager::MOVINGBOMB)
        .value("Movingbrush", DimensionManager::MOVINGBRUSH)
        .value("Greenbomb", DimensionManager::GREENBOMB)
        .value("Director", DimensionManager::DIRECTOR)

        .value("No_Dim_Check", DimensionManager::NO_DIM_CHECK)
        .value("No_Crate_Check", DimensionManager::NO_CRATE_CHECK)
        .value("No_Moving_Check", DimensionManager::NO_MOVING_CHECK)
        .value("No_Bullet_Hit", DimensionManager::NO_BULLET_HIT)
        .value("Invisible", DimensionManager::INVISIBLE)
        .value("Liftable", DimensionManager::LIFTABLE)
        ;

    //Show message box.
    py::def("showMessage", py_showMessage,
            "showMessage(message, time)\n"
            "Shows a message. If time > 0, the message fades after the time (in seconds) elapses."
            );

    //Switch World.
    py::def("nextWorld", py_nextWorld,
            "nextWorld()\n"
            "Go to the next NGF World."
            );
    py::def("previousWorld", py_previousWorld,
            "previousWorld()\n"
            "Go to the previous NGF World."
            );
    py::def("gotoWorld", py_gotoWorld,
            "gotoWorld(n)\n"
            "Go to the NGF World with the given index (0 = first)."
            );
    py::def("winLevel", py_winLevel,
            "winLevel()\n"
            "The current level is 'completed'."
            );
    py::def("loseLevel", py_loseLevel,
            "loseLevel()\n"
            "The current level has been 'lost'."
            );

    //NGF level load/clear.
    py::def("loadNgf", py_loadNgf,
            "loadNgf(name)\n"
            "Loads the NGF level with the given name, as specified in the .ngf file. or the 'scene name' in Blender. If you want"
            "to replace the current scene, remember to clear the level (GraLL2.clearLevel()) first."
           );
    py::def("clearLevel", py_clearLevel,
            "clearLevel()\n"
            "Clears the level (destroys all objects)."
           );

    //Serialise/deserialise.
    py::def("serialise", py_serialise,
            "serialise(name)\n"
            "Saves the game state to a file with the given name and extension '.sav' under 'Saves' in the user directory."
           );
    py::def("deserialise", py_deserialise,
            "deserialise(name)\n"
            "Restores the game state from a file with the given name and extension '.sav' under 'Saves' in the user directory."
           );

    //Dimension stuff
    py::def("switchDimension", py_switchDimension,
            "switchDimension()\n"
            "Makes GraLL switch dimensions."
            );
    py::def("setDimension", py_setDimension,
            "setDimension(n)\n"
            "Sets the global dimension to the given one (1 or 2)."
            );

    //Fades.
    py::def("fadeInColour", py_fadeInScreen,
            "fadeInColour(time)\n"
            "Fades in the the fade colour in given time."
            );
    py::def("fadeOutColour", py_fadeOutScreen,
            "fadeOutColour(time)\n"
            "Fades out the the fade colour in given time."
            );
    py::def("fadeInOutColour", py_fadeInOutScreen,
            "fadeInOutColour(in,pause,out)\n"
            "Fades in and out the colour in respective times, with a pause in between."
            );

    //Level stuff.
    py::def("saveExists", py_saveExists,
            "saveExists(n)\n"
            "Whether save for level n exists."
            );
    py::def("removeSave", py_removeSave,
            "removeSave(n)\n"
            "Remove level n's save."
            );
    py::def("getCurrentLevelNum", py_getCurrentLevelNum,
            "getCurrentLevelNum()\n"
            "Returns the number of the currently running level."
            );

    //Music stuff.
    py::def("playMusic", py_playMusic,
            "playMusic(track)\n"
            "Plays music track given."
            );
    py::def("crossFadeMusic", py_crossFadeMusic,
            "crossFadeMusic(track,time)\n"
            "Crossfades from playing track to given track in given time."
            );
    py::def("setMusicGain", py_setMusicGain,
            "setMusicGain(n)\n"
            "Sets gain (volume) of currently playing track."
            );
    py::def("getMusicGain", py_getMusicGain,
            "getMusicGain()\n"
            "Gets gain (volume) of currently playing track."
            );

    //Video recording stuff.
    py::def("startRecordingVideo", py_startRecordingVideo,
            "startRecordingVideo(p)\n"
            "Starts recording video with the given time period between each frame."
            );
    py::def("stopRecordingVideo", py_stopRecordingVideo,
            "stopRecordingVideo()\n"
            "Stops recording the video."
            );

    //Control setting.
    py::def("setControl", py_setControl,
            "setControl(control,key)\n"
            "Sets the given control to the given key, where 'control' is one of:\n"
            "   'forward'\n"
            "   'backward'\n"
            "   'left'\n"
            "   'right'\n"
            "   'dimensionSwitch'\n"
            "   'selfDestruct'\n"
            "   'peepLeft'\n"
            "   'peepRight'\n"
            "Possible 'key' values are the strings displayed in the 'Options' dialog in the 'Controls' tab for the keyboard controls."
            );
    py::def("getControl", py_getControl,
            "getControl(control)\n"
            "Gets the key for the given control, where 'control' is one of:\n"
            "   'forward'\n"
            "   'backward'\n"
            "   'left'\n"
            "   'right'\n"
            "   'dimensionSwitch'\n"
            "   'selfDestruct'\n"
            "   'peepLeft'\n"
            "   'peepRight'"
            );

    //PythonObject clicking.
    py::def("clickObject", py_clickObject,
            "clickObject()\n"
            "Goes into 'click' mode. The clicked object can be retieved using 'getClickedObject()'. It is also saved in the global"
            "'clicked' variable. Use carefully! If the clicked object was destroyed this will cause the game to crash!"
            );
    py::def("getClickedObject", py_getClickedObject,
            "getClickedObject()\n"
            "Retrieve the clicked object. The object is also saved in the global 'clicked' variable. Use carefully! If the clicked"
            "object was destroyed, this will cause the game to crash!"
            );

    //Screenshots.
    py::def("screenshot", py_screenshot,
            "screenshot(highres,name,extension)\n"
            "Takes a screenshot. The file is saved under the 'Screenshots' in the GraLL 2 user directory, with the given name and"
            "extension ('.jpg', '.png'). If highres is true, a high-resolution screenshot is taken."
           );

    //Bonus time.
    py::def("setBonusTime", py_setBonusTime,
            "setBonusTime(time)\n"
            "Sets the bonus time to given time."
           );
    py::def("getBonusTime", py_getBonusTime,
            "getBonusTime()\n"
            "Returns the bonus time."
           );

    //Slideshow
    py::def("startSlideshow", py_startSlideshow,
            "startSlideshow(width, height, slides)\n"
            "Starts playing a slideshow. The 'slides' parameter is a list of tuples, with times and texture names. For example,"
            "[(0.5,'Slide1.png'),(3,'Slide2.png')] etc. The 'width' and 'height' parameters describe the width and height of the"
            "images."
           );
}

//--- The function that gets called from Game::init() ---------------------------

void initPythonBinds()
{
    initGraLL2();
}

