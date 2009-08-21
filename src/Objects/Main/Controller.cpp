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

    //Load script.
    NGF::Python::PythonGameObject::setScript(mProperties.getValue("script", 0, ""));
    NGF_PY_SAVE_EVENT(levelStart);
    NGF_PY_SAVE_EVENT(levelStop);

    //Init.
    NGF_PY_CALL_EVENT(init);

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
void Controller::postLoad()
{
    //Create.
    NGF_PY_CALL_EVENT(create);
}
//-------------------------------------------------------------------------------
NGF::MessageReply Controller::receiveMessage(NGF::Message msg)
{
    switch (msg.code)
    {
        case MSG_LEVELSTART:
            mPythonEvents["levelStart"](); //No 'self' parameter.
            NGF_NO_REPLY();

        case MSG_LEVELSTOP:
            mPythonEvents["levelStop"]();
            NGF_NO_REPLY();
    }

    NGF_NO_REPLY();
}
//-------------------------------------------------------------------------------
