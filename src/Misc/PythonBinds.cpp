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
    NGF::GameObject *msg = showMessage(message, time);
    NGF::Python::PythonGameObject *pyMsg = dynamic_cast<NGF::Python::PythonGameObject*>(msg);
    return pyMsg->getConnector();
}

//World switching.
void py_nextWorld() 
{ 
    nextWorld();
}
void py_previousWorld() 
{ 
    previousWorld(); 
}
void py_gotoWorld(int index) 
{ 
    gotoWorld(index); 
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

//Saved-game stuff.
void py_deleteSave()
{
    remove((USER_PREFIX + "Saves/" + GlbVar.levelName).c_str());
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

//--- The module ----------------------------------------------------------------

BOOST_PYTHON_MODULE(GraLL2)
{
    //Show message box.
    py::def("showMessage", py_showMessage);

    //Switch World.
    py::def("nextWorld", py_nextWorld);
    py::def("previousWorld", py_previousWorld);
    py::def("gotoWorld", py_gotoWorld);

    //Dimension stuff (if you want to switch dimensions, best you do it
    //through the Player object).
    py::def("switchDimension", py_switchDimension);
    py::def("setDimension", py_setDimension);

    //Fades.
    py::def("fadeInColour", py_fadeInScreen);
    py::def("fadeOutColour", py_fadeOutScreen);
    py::def("fadeInOutColour", py_fadeInOutScreen);

    //Saved-game stuff.
    py::def("deleteSave", py_deleteSave);

    //Music stuff.
    py::def("playMusic", py_playMusic);
    py::def("crossFadeMusic", py_crossFadeMusic);
    py::def("setMusicGain", py_setMusicGain);
    py::def("getMusicGain", py_getMusicGain);

    //Video recording stuff.
    py::def("startRecordingVideo", py_startRecordingVideo);
    py::def("stopRecordingVideo", py_stopRecordingVideo);
}

//--- The function that gets called from Game::init() ---------------------------

void initPythonBinds()
{
    initGraLL2();
}
