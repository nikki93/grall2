/*
=========================================
Point.cpp
=========================================
*/

#define __POINT_CPP__

#include "Objects/Misc/Point.h"

//--- NGF events ----------------------------------------------------------------
Point::Point(Ogre::Vector3 pos, Ogre::Quaternion rot, NGF::ID id, NGF::PropertyList properties, Ogre::String name)
    : NGF::GameObject(pos, rot, id , properties, name)
{
    Ogre::String ogreName = "id" + Ogre::StringConverter::toString(getID());
    addFlag("Point");

    //Create the SceneNode.
    mNode = GlbVar.ogreSmgr->getRootSceneNode()->createChildSceneNode(ogreName, pos);

    //Load the Python script.
    SET_PYTHON_SCRIPT();

    //Python init event.
    NGF_PY_CALL_EVENT(init);
}
//-------------------------------------------------------------------------------
void Point::postLoad()
{
    //Python create event.
    NGF_PY_CALL_EVENT(create);
}
//-------------------------------------------------------------------------------
Point::~Point()
{
    GlbVar.ogreSmgr->destroySceneNode(mNode);
}
//-------------------------------------------------------------------------------
void Point::unpausedTick(const Ogre::FrameEvent &evt)
{
    //Python utick event.
    NGF_PY_CALL_EVENT(utick, evt.timeSinceLastFrame);
}
//-------------------------------------------------------------------------------
void Point::pausedTick(const Ogre::FrameEvent &evt)
{
    //Python ptick event.
    NGF_PY_CALL_EVENT(ptick, evt.timeSinceLastFrame);
}
//-------------------------------------------------------------------------------
NGF::MessageReply Point::receiveMessage(NGF::Message msg)
{
    switch (msg.code)
    {
        case MSG_GETPOSITION:
            NGF_SEND_REPLY(mNode->getPosition());
    }
    NGF_NO_REPLY();
}
//-------------------------------------------------------------------------------

//--- Python interface implementation -------------------------------------------
NGF_PY_BEGIN_IMPL(Point)
{
    NGF_PY_METHOD_IMPL(getPosition)
    { 
        NGF_PY_RETURN(mNode->getPosition());
    }
    NGF_PY_METHOD_IMPL(setPosition)
    { 
        mNode->setPosition(py::extract<Ogre::Vector3>(args[0]));

        NGF_PY_RETURN();
    }
    NGF_PY_METHOD_IMPL(translate)
    {
        Ogre::Vector3 trans = py::extract<Ogre::Vector3>(args[0]);
        mNode->translate(trans);

        NGF_PY_RETURN();
    }
}
NGF_PY_END_IMPL
//-------------------------------------------------------------------------------
