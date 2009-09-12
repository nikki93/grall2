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
void py_fadeInScreen(Ogre::Real time) 
{ 
    GlbVar.fader->fadeIn(time); 
}
void py_fadeOutScreen(Ogre::Real time) 
{ 
    GlbVar.fader->fadeOut(time); 
}
void py_fadeInOutScreen(Ogre::Real in, Ogre::Real pause, Ogre::Real out) 
{ 
    GlbVar.fader->fadeInOut(in, pause, out); 
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


//--- The module ----------------------------------------------------------------

BOOST_PYTHON_MODULE(GraLL2)
{
    //Docstring settings.
    py::docstring_options doc_options(true, true, false);

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

    //Dimension stuff (if you want to switch dimensions, best you do it
    //through the Player object).
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
}

//--- The function that gets called from Game::init() ---------------------------

void initPythonBinds()
{
    initGraLL2();
}

