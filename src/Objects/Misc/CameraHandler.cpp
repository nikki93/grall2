/*
=========================================
CameraHandler.cpp
=========================================
*/

#define __CAMERAHANDLER_CPP__

#include "Objects/Misc/CameraHandler.h"

//--- NGF events ----------------------------------------------------------------
CameraHandler::CameraHandler(Ogre::Vector3 pos, Ogre::Quaternion rot, NGF::ID id, NGF::PropertyList properties, Ogre::String name)
    : NGF::GameObject(pos, rot, id , properties, name),
      mCamera(GlbVar.ogreCamera),
      mCurrState(CS_NONE),
      mTargetNode(NULL),
      mTargetNodeName(""),
      mOffset(0,4.5,8),
      mMovementFactor(1),
      mRotationFactor(0)
{
    Ogre::String ogreName = "id" + Ogre::StringConverter::toString(getID());
    addFlag("CameraHandler");

    //Load the Python script.
    NGF::Python::PythonGameObject::setScript(mProperties.getValue("script", 0, ""));

    //Python init event.
    NGF_PY_CALL_EVENT(init);

    //Tell the old guy to get out.
    if (GlbVar.currCameraHandler)
        GlbVar.goMgr->destroyObject(GlbVar.currCameraHandler->getID());
    GlbVar.currCameraHandler = this;

    //Attach Camera.
    Ogre::SceneNode *prev = mCamera->getParentSceneNode();
    if (prev && prev != GlbVar.ogreSmgr->getRootSceneNode())
        prev->detachObject(mCamera);

    mCamera->setPosition(pos);
    mCamera->setOrientation(rot);

    mCameraNode = GlbVar.ogreSmgr->getRootSceneNode()->createChildSceneNode(ogreName, pos, rot);
}
//-------------------------------------------------------------------------------
void CameraHandler::postLoad()
{
    //Python create event.
    NGF_PY_CALL_EVENT(create);
}
//-------------------------------------------------------------------------------
CameraHandler::~CameraHandler()
{
    mTargetNode = 0;
    GlbVar.ogreSmgr->destroySceneNode(mCameraNode);
    GlbVar.currCameraHandler = 0;
}
//-------------------------------------------------------------------------------
void CameraHandler::unpausedTick(const Ogre::FrameEvent &evt)
{
    //Node-remembering hack.
    if (mTargetNodeName != "")
    {
        mTargetNode = mTargetNodeName == "NULL" ? NULL : GlbVar.ogreSmgr->getSceneNode(mTargetNodeName);
        mTargetNodeName = "";
    }

    Ogre::Vector3 effOffset = mOffset;

    if (isKeyDown(OIS::KC_Q))
        effOffset = Ogre::Vector3(5,4.5,0);
    if (isKeyDown(OIS::KC_E))
    {
        effOffset = Ogre::Vector3(-5,4.5,0);
        if (isKeyDown(OIS::KC_Q))
            effOffset = Ogre::Vector3(0,4.5,-5);
    }

    //Do the camera-handling based on state.
    switch (mCurrState)
    {
        case CS_THIRDPERSON:
            {
                Ogre::Vector3 target = mTargetNode->getPosition() + (mTargetNode->getOrientation() * effOffset);
                Ogre::Vector3 toMove = (target - mCamera->getPosition()) * mMovementFactor * evt.timeSinceLastFrame;
                mCamera->move(toMove);
            }
            lookAt(mTargetNode->getPosition() + Ogre::Vector3(0,1,0), evt.timeSinceLastFrame);
            break;
        case CS_LOOKAT:
            lookAt(mTargetNode->getPosition(), evt.timeSinceLastFrame);
            break;
        case CS_NONE:
            mTargetNode = 0;
            break;
    }

    //Python utick event.
    NGF_PY_CALL_EVENT(utick, evt.timeSinceLastFrame);

    //Tasks.
    updateTasks(evt.timeSinceLastFrame);
}
//-------------------------------------------------------------------------------
void CameraHandler::pausedTick(const Ogre::FrameEvent &evt)
{
    //Python ptick event.
    NGF_PY_CALL_EVENT(ptick, evt.timeSinceLastFrame);
}
//-------------------------------------------------------------------------------
NGF::MessageReply CameraHandler::receiveMessage(NGF::Message msg)
{
    switch (msg.code)
    {
        case MSG_SETTARGET:
            mTargetNode = msg.getParam<Ogre::SceneNode*>(0);
            break;

        case MSG_SETOFFSET:
            mOffset = msg.getParam<Ogre::Vector3>(0);
            break;

        case MSG_SETSMOOTHINGFACTOR:
            mMovementFactor = msg.getParam<Ogre::Real>(0);
            break;

        case MSG_SETCAMERASTATE:
            mCurrState = msg.getParam<int>(0);

            switch (mCurrState)
            {
                case CS_NONE:
                    mTargetNode = 0;
                    break;
            }
            break;
    }
    NGF_NO_REPLY();
}
//-------------------------------------------------------------------------------

//--- Python interface implementation -------------------------------------------
NGF_PY_BEGIN_IMPL(CameraHandler)
{
    NGF_PY_METHOD_IMPL(setTarget)
    { 
        NGF::Python::PythonObjectConnectorPtr obj = py::extract<NGF::Python::PythonObjectConnectorPtr>(args[0]);
        NGF::ID id = obj->getID();
        mTargetNode = GlbVar.ogreSmgr->getSceneNode("id" + Ogre::StringConverter::toString(id));

        NGF_PY_RETURN();
    }
    NGF_PY_METHOD_IMPL(targetPlayer)
    { 
        NGF::Python::PythonObjectConnectorPtr obj = py::extract<NGF::Python::PythonObjectConnectorPtr>(args[0]);
        NGF::ID id = obj->getID();
        mTargetNode = GlbVar.ogreSmgr->getSceneNode("id" + Ogre::StringConverter::toString(id) + "-controlnode");

        NGF_PY_RETURN();
    }

    NGF_EXTRAS_PYTHON_METHOD_ADD_TASK(addTask);

    NGF_PY_PROPERTY_IMPL(currState, mCurrState, int)
    NGF_PY_PROPERTY_IMPL(movementFactor, mMovementFactor, Ogre::Real)
    NGF_PY_PROPERTY_IMPL(rotationFactor, mRotationFactor, Ogre::Real)
    NGF_PY_PROPERTY_IMPL(offset, mOffset, Ogre::Vector3)
}
NGF_PY_END_IMPL
//-------------------------------------------------------------------------------
