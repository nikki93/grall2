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
      mOffset(0,5,-10),
      mSmoothingFactor(0.2)
{
    Ogre::String ogreName = "id" + Ogre::StringConverter::toString(getID());
    addFlag("CameraHandler");

    //Load the Python script.
    NGF::Python::PythonGameObject::setScript(mProperties.getValue("script", 0, ""));

    //Python init event.
    NGF_PY_CALL_EVENT(init);

    //Attach Camera.
    Ogre::SceneNode *prev = mCamera->getParentSceneNode();
    if (prev && prev != GlbVar.ogreSmgr->getRootSceneNode())
        prev->detachObject(mCamera);

    mCameraNode = GlbVar.ogreSmgr->getRootSceneNode()->createChildSceneNode(ogreName, pos, rot);
    mCameraNode->attachObject(mCamera);

    //Python create event.
    NGF_PY_CALL_EVENT(create);
}
//-------------------------------------------------------------------------------
CameraHandler::~CameraHandler()
{
    mCameraNode->detachAllObjects();
    GlbVar.ogreSmgr->destroySceneNode(mCameraNode);
}
//-------------------------------------------------------------------------------
void CameraHandler::unpausedTick(const Ogre::FrameEvent &evt)
{
    switch (mCurrState)
    {
        case CS_THIRDPERSON:
            if (mTargetNode)
            {
                Ogre::Vector3 target = mTargetNode->getPosition() + (mTargetNode->getOrientation() * mOffset);
                Ogre::Vector3 toMove = (target - mCameraNode->getPosition()) * mSmoothingFactor * evt.timeSinceLastFrame;
                mCameraNode->translate(toMove);
                lookAt(mTargetNode->getPosition() + Ogre::Vector3(0,1,0));
            }
            break;
        case CS_LOOKAT:
            if (mTargetNode)
            {
                lookAt(mTargetNode->getPosition());
            }
            break;
            //TODO: Handle spline.
    }
    //Python utick event.
    NGF_PY_CALL_EVENT(utick, evt.timeSinceLastFrame);
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
            mCameraNode->setOrientation(mTargetNode->getOrientation());
            break;

        case MSG_SETOFFSET:
            mOffset = msg.getParam<Ogre::Vector3>(0);
            break;

        case MSG_SETSMOOTHINGFACTOR:
            mSmoothingFactor = msg.getParam<Ogre::Real>(0);
            break;

        case MSG_SETCAMERASTATE:
            mCurrState = msg.getParam<int>(0);
            break;
    }
    NGF_NO_REPLY();
}
//-------------------------------------------------------------------------------
