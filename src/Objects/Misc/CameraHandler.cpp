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
      mViewAngle(30),
      mSplineAnim(NULL),
      mSplineAnimState(NULL),
      mSplineTrack(NULL),
      mRestartTime(-1),
      mDeathTime(-1)
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
    OIS::MouseState ms = getMouseState();
    if (ms.buttonDown(OIS::MB_Right) && mCurrState == CS_THIRDPERSON)
    {
        Ogre::Real yRel = -ms.Y.rel * 0.1;
        int invert = GlbVar.settings.controls.invertMouse ? -1 : 1;
        mViewAngle -= Ogre::Degree(yRel) * GlbVar.settings.controls.upDownSensitivity * invert;
        mViewAngle = clamp<Ogre::Real>(mViewAngle.valueDegrees(), 10, 70);
    }

    mViewOffset = Ogre::Vector3(0, Ogre::Math::Sin(mViewAngle), Ogre::Math::Cos(mViewAngle)) * 10;
    mLookAtOffset = Ogre::Vector3(0,2,0);

    if (mViewAngle > Ogre::Degree(50))
    {
        mViewOffset.y *= ((mViewOffset.y - 7.66) * 0.2) + 1;
    }
    else if (mViewAngle < Ogre::Degree(27))
    {
        Ogre::Real diff = 27 - mViewAngle.valueDegrees();
        Ogre::Real fact = 0;

        //Smoothstep. :-)
        Ogre::Real max = 17, min = 0;
        if (diff < min)
            fact = 0;
        else if (diff >= max)
            fact = 1;
        else 
            fact = (3 * ((diff - min) / (max - min)) * ((diff - min) / (max - min))) - (2 * ((diff - min) / (max - min)) * ((diff - min) / (max - min)) * ((diff - min) / (max - min)));

        mViewOffset.z -= fact;
        mLookAtOffset.y += 3 * fact;
        mViewOffset.y = 3.42 - ((3.42 - mViewOffset.y) * 1.2);
    }

    //Peeping.
    if (isKeyDown(GlbVar.settings.controls.peepLeft))
        mViewOffset = Ogre::Vector3(5,4.5,0);
    if (isKeyDown(GlbVar.settings.controls.peepRight))
    {
        mViewOffset = Ogre::Vector3(-5,4.5,0);
        if (isKeyDown(GlbVar.settings.controls.peepLeft))
            mViewOffset = Ogre::Vector3(0,4.5,-5);
    }

    //Python utick event (do it before camera handling to allow offset-modifications).
    NGF_PY_CALL_EVENT(utick, evt.timeSinceLastFrame);

    //Do the camera-handling based on state.
    switch (mCurrState)
    {
        case CS_THIRDPERSON:
            {
                //Weird stuff here.

                Ogre::Vector3 target = mTargetNode->getPosition() + (mTargetNode->getOrientation() * mViewOffset);
                Ogre::Real factor = mMovementFactor;
                Ogre::Vector3 lookAtOffset = mLookAtOffset;

                //Raycast from target to us, then if hit, go to hit point, but 0.5 units closer.
                if (GlbVar.settings.misc.fixCameraObstruction)
                {
                    Ogre::Vector3 dir = mTargetNode->getPosition() - target;

                    btVector3 from = BtOgre::Convert::toBullet(mTargetNode->getPosition() + 
                            (Ogre::Vector3(4,0,0).getRotationTo(-dir) * Ogre::Vector3(4,0,0)));
                    btVector3 to = BtOgre::Convert::toBullet(target);

                    struct CameraTest : public btCollisionWorld::ClosestRayResultCallback
                    {
                        CameraTest(const btVector3 &from, const btVector3 &to)
                            : btCollisionWorld::ClosestRayResultCallback(from, to)
                        {
                        }

                        bool needsCollision(btBroadphaseProxy *proxy0) const
                        {
                            return proxy0->m_collisionFilterGroup & GlbVar.dimMgr->getCurrentDimension() //Viewed dimension only.
                                && !(((btCollisionObject*) proxy0->m_clientObject)->getCollisionFlags() & btCollisionObject::CF_NO_CONTACT_RESPONSE); //If no contact response, ignore.
                        }
                    } res(from, to);

                    GlbVar.phyWorld->rayTest(from, to, res);
                    if (res.m_collisionObject)
                    {
                        Ogre::Vector3 hitPoint = BtOgre::Convert::toOgre(res.m_hitPointWorld);
                        target = hitPoint + (Ogre::Vector3(0.5,0,0).getRotationTo(dir) * Ogre::Vector3(0.5,0,0));
                        factor = 2.8; //<-- Dunno, makes things better. O_o
                        lookAtOffset *= 0.7;
                    }
                }

                //Smoothly move to target.
                Ogre::Vector3 toMove = (target - mCamera->getPosition()) * factor * evt.timeSinceLastFrame;
                mCamera->move(toMove);

                //Gotta look at the guy. :P
                lookAt(mTargetNode->getPosition() + lookAtOffset, evt.timeSinceLastFrame);
            }
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

        case CS_DEATH:
            {
                //Move it up, rotate it.
                mDeathOffset.y += 4 * evt.timeSinceLastFrame;
                mDeathOffset = Ogre::Quaternion(Ogre::Degree(20) * evt.timeSinceLastFrame, Ogre::Vector3::UNIT_Y) * mDeathOffset;

                //Put the Camera there, make him look at the point.
                Ogre::Vector3 toMove = ((mDeathLastPos + mDeathOffset) - mCamera->getPosition()) * 4 * evt.timeSinceLastFrame;
                mCamera->move(toMove);
                lookAt(mDeathLastPos + mLookAtOffset, evt.timeSinceLastFrame);

                mDeathTime -= evt.timeSinceLastFrame;

                //If it's time, restart world.
                if (mDeathTime <= 0)
                {
                    GlbVar.fader->fadeInOut(0.7,0.4,0.5);
                    mRestartTime = 1.1;

                    mDeathTime = 777; //Stupid hack.
                }
            }
            break;
    }

    //Restarting.
    if (mRestartTime > 0)
    {
        mRestartTime -= evt.timeSinceLastFrame;

        if (mRestartTime <= 0)
            gotoWorld(GlbVar.woMgr->getCurrentWorldIndex());
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
                case CS_DEATH:
                    mDeathLastPos = mTargetNode ? mTargetNode->getPosition() : Ogre::Vector3::ZERO;
                    mDeathOffset = mTargetNode->getOrientation() * Ogre::Vector3(0, mViewOffset.y + 1, 3);
                    mTargetNode = 0;
                    mDeathTime = 2;
                    break;
            }
            break;

        case MSG_GETTARGET:
            NGF_SEND_REPLY(mTargetNode);
            break;

        case MSG_TELEPORT:
            //Get current offset, and then move to the teleported position with same offset.
            Ogre::Vector3 currOffset;
            if (mTargetNode)
                currOffset = mCamera->getPosition() - mTargetNode->getPosition();
            else
                currOffset = 0;
            Ogre::Vector3 newPos = msg.getParam<Ogre::Vector3>(0) + currOffset;
            mCamera->setPosition(newPos);
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
}
NGF_PY_END_IMPL
//-------------------------------------------------------------------------------
