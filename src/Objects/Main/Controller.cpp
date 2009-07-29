/*
=========================================
Controller.cpp
=========================================
*/

#define __CONTROLLER_CPP__

#include "Objects/Main/Controller.h"

//--- NGF events ----------------------------------------------------------------
Controller::Controller(Ogre::Vector3 pos, Ogre::Quaternion rot, NGF::ID id, NGF::PropertyList properties, Ogre::String name)
    : NGF::GameObject(pos, rot, id , properties, name)
{
    addFlag("Controller");

    //Old guy, out!
    if (GlbVar.controller)
        GlbVar.goMgr->destroyObject(GlbVar.controller->getID());
    GlbVar.controller = this;

    //We stick around. :-)
    setPersistent(true);
}
//-------------------------------------------------------------------------------
Controller::~Controller()
{
    GlbVar.controller = NULL;
}
//-------------------------------------------------------------------------------
