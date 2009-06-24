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
    GlbVar.worldSwitch = GlbVar.woMgr->getCurrentWorldIndex() + 1; 
}
void py_previousWorld() 
{ 
    GlbVar.worldSwitch = GlbVar.woMgr->getCurrentWorldIndex() - 1; 
}
void py_gotoWorld(int index) 
{ 
    GlbVar.worldSwitch = index; 
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
void py_fadeIn(Ogre::Real time) 
{ 
    GlbVar.fader->fadeIn(time); 
}
void py_fadeOut(Ogre::Real time) 
{ 
    GlbVar.fader->fadeOut(time); 
}
void py_fadeInOut(Ogre::Real in, Ogre::Real pause, Ogre::Real out) 
{ 
    GlbVar.fader->fadeInOut(in, pause, out); 
}

//Saved-game stuff.
void py_deleteSave()
{
    remove((USER_PREFIX + "Saves/" + GlbVar.levelName).c_str());
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
    py::def("fadeIn", py_fadeIn);
    py::def("fadeOut", py_fadeOut);
    py::def("fadeInOut", py_fadeInOut);

    //Saved-game stuff.
    py::def("deleteSave", py_deleteSave);
}

//--- The function that gets called from Game::init() ---------------------------

void initPythonBinds()
{
    initGraLL2();
}
