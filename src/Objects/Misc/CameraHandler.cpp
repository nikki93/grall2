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
      mMovementFactor(4),
      mRotationFactor(0),
      mCameraHeight(4.5),
      mSplineAnim(NULL),
      mSplineAnimState(NULL),
      mSplineTrack(NULL)
{
    Ogre::String ogreName = "id" + Ogre::StringConverter::toString(getID());
    addFlag("CameraHandler");

    //Load the Python script.
    NGF::Python::PythonGameObject::setScript(mProperties.getValue("script", 0, ""));
    NGF_PY_SAVE_EVENT(alarm);

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

    //Create the spline node.
    mSplineNode = GlbVar.ogreSmgr->getRootSceneNode()->createChildSceneNode(ogreName, pos);
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
    if (mSplineAnim)
        GlbVar.ogreSmgr->destroyAnimation(mSplineAnim->getName());
    if (mSplineAnimState)
        GlbVar.ogreSmgr->destroyAnimationState(mSplineAnimState->getAnimationName());
    if (mSplineTrack)
        mSplineAnim->destroyNodeTrack(0);
    mTargetNode = 0;
    GlbVar.ogreSmgr->destroySceneNode(mSplineNode);
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

    //Third-person view offset handling.

    //Change height.
    if (mCurrState == CS_THIRDPERSON)
    {
        mCameraHeight += isKeyDown(OIS::KC_R) * 6 * evt.timeSinceLastFrame;
        mCameraHeight -= isKeyDown(OIS::KC_F) * 6 * evt.timeSinceLastFrame;
        mCameraHeight = clamp<Ogre::Real>(mCameraHeight, 4.5, 12);
    }

    Ogre::Vector3 mOffset = Ogre::Vector3(0, mCameraHeight, (mCameraHeight > 10) ? 6 : 8);

    //Peeping.
    if (isKeyDown(OIS::KC_Q))
        mOffset = Ogre::Vector3(5,4.5,0);
    if (isKeyDown(OIS::KC_E))
    {
        mOffset = Ogre::Vector3(-5,4.5,0);
        if (isKeyDown(OIS::KC_Q))
            mOffset = Ogre::Vector3(0,4.5,-5);
    }

    //Python utick event (do it before camera handling to allow offset-modifications).
    NGF_PY_CALL_EVENT(utick, evt.timeSinceLastFrame);

    //Do the camera-handling based on state.
    switch (mCurrState)
    {
        case CS_THIRDPERSON:
            {
                Ogre::Vector3 target = mTargetNode->getPosition() + (mTargetNode->getOrientation() * mOffset);
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
        case CS_SPLINE:
            if (mTargetNode)
                lookAt(mTargetNode->getPosition(), evt.timeSinceLastFrame);
            if (mSplineAnim)
            {
                mSplineAnimState->addTime(evt.timeSinceLastFrame);
                mCamera->setPosition(mSplineNode->getPosition());
            }

            break;
    }

    //Alarms.
    updateAlarms(evt.timeSinceLastFrame);
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
    NGF_PY_METHOD_IMPL(setAlarm)                                                                  
    {                                                                                          
        setAlarm(py::extract<float>(args[0]), py::extract<Alarm>(args[1]));                    
        float  f = py::extract<float>(args[0]);
                                                                                               
        return py::object();                                                                   
    }

    NGF_PY_METHOD_IMPL(getPosition)
    { 
        NGF_PY_RETURN(mCamera->getPosition());
    }
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
    NGF_PY_METHOD_IMPL(beginSpline)
    {
        if (mSplineAnim)
            GlbVar.ogreSmgr->destroyAnimation(mSplineAnim->getName());
        if (mSplineAnimState)
            GlbVar.ogreSmgr->destroyAnimationState(mSplineAnimState->getAnimationName());
        if (mSplineTrack)
            mSplineAnim->destroyNodeTrack(0);

        mSplineAnim = GlbVar.ogreSmgr->createAnimation("cameraSpline", py::extract<Ogre::Real>(args[0]));
        mSplineAnim->setInterpolationMode(Ogre::Animation::IM_SPLINE);

        mSplineTrack = mSplineAnim->createNodeTrack(0, mSplineNode);

        NGF_PY_RETURN();
    }
    NGF_PY_METHOD_IMPL(addSplinePoint)
    {
        Ogre::TransformKeyFrame *key = mSplineTrack->createNodeKeyFrame(py::extract<Ogre::Real>(args[0]));
        key->setTranslate(py::extract<Ogre::Vector3>(args[1]));

        NGF_PY_RETURN();
    }
    NGF_PY_METHOD_IMPL(endSpline)
    {
        mSplineAnimState = GlbVar.ogreSmgr->createAnimationState("cameraSpline");
        mSplineAnimState->setEnabled(true);
        mSplineAnimState->setLoop(false);

        NGF_PY_RETURN();
    }

    NGF_PY_PROPERTY_IMPL(currState, mCurrState, int)
    NGF_PY_PROPERTY_IMPL(movementFactor, mMovementFactor, Ogre::Real)
    NGF_PY_PROPERTY_IMPL(rotationFactor, mRotationFactor, Ogre::Real)
    NGF_PY_PROPERTY_IMPL(offset, mOffset, Ogre::Vector3)
}
NGF_PY_END_IMPL
//-------------------------------------------------------------------------------
