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
      mCamNode(GlbVar.camNode),
      mCurrState(CS_NONE),
      mTargetNode(NULL),
      mTargetNodeName(""),
      mMovementFactor(4),
      mRotationFactor(0),
      mViewAngle(30),
      mSplineAnim(NULL),
      mSplineAnimState(NULL),
      mSplineTrack(NULL),
      mLastKeyFrameTime(0),
      mViewRoll(Ogre::Quaternion::IDENTITY)
{
    Ogre::String ogreName = "id" + Ogre::StringConverter::toString(getID());
    addFlag("CameraHandler");

    //Set the Python us.
    py::object &main = NGF::Python::PythonManager::getSingleton().getMainNamespace();
    py::exec(
            "import GraLL2\n\n"

            "def setCameraHandler(obj):\n"
            "  GraLL2.cameraHandler = obj\n",
            main, main
            ); 
    main["setCameraHandler"](getConnector());
    py::exec(
            "del setCameraHandler\n",
            main, main
            );

    //Load the Python script.
    SET_PYTHON_SCRIPT();

    NGF_PY_SAVE_EVENT(alarm);

    //Python init event.
    NGF_PY_CALL_EVENT(init);

    //Tell the old guy to get out.
    if (GlbVar.currCameraHandler)
        GlbVar.goMgr->destroyObject(GlbVar.currCameraHandler->getID());
    GlbVar.currCameraHandler = this;

    //Set initial position.
    mCamNode->setPosition(pos);
    mCamNode->setOrientation(rot);

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
    if (mSplineTrack)
        mSplineAnim->destroyNodeTrack(0);
    if (mSplineAnim)
        GlbVar.ogreSmgr->destroyAnimation(mSplineAnim->getName());
    if (mSplineAnimState)
        GlbVar.ogreSmgr->destroyAnimationState(mSplineAnimState->getAnimationName());
    mTargetNode = 0;
    GlbVar.ogreSmgr->destroySceneNode(mSplineNode);
    GlbVar.currCameraHandler = 0;
}
//-------------------------------------------------------------------------------
void CameraHandler::unpausedTick(const Ogre::FrameEvent &evt)
{
    //Node-remembering hack.
    /*
    if (mTargetNodeName != "")
    {
        mTargetNode = mTargetNodeName == "NULL" ? NULL : GlbVar.ogreSmgr->getSceneNode(mTargetNodeName);
        mTargetNodeName = "";
    }
    */

    //Peeping.
    if (Util::isKeyDown(GlbVar.settings.controls.keys["peepLeft"]) && Util::isKeyDown(GlbVar.settings.controls.keys["peepRight"]))
        mViewOffset = Ogre::Vector3(0,4.5,-5);
    else if (Util::isKeyDown(GlbVar.settings.controls.keys["peepLeft"]))
        mViewOffset = Ogre::Vector3(5,4.5,0);
    else if (Util::isKeyDown(GlbVar.settings.controls.keys["peepRight"]))
        mViewOffset = Ogre::Vector3(-5,4.5,0);
    else
    {
        //Third-person view offset handling.
        OIS::MouseState ms = Util::getMouseState();
        if (ms.buttonDown(OIS::MB_Right) && mCurrState == CS_THIRDPERSON)
        {
            Ogre::Real yRel = -ms.Y.rel * 0.1;
            int invert = GlbVar.settings.controls.invertMouse ? -1 : 1;
            mViewAngle -= Ogre::Degree(yRel) * GlbVar.settings.controls.upDownSensitivity * invert;
            mViewAngle = Util::clamp<Ogre::Real>(mViewAngle.valueDegrees(), 10, 75);
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
                fact = (3 * ((diff - min) / (max - min)) * ((diff - min) / (max - min))) 
                    - (2 * ((diff - min) / (max - min)) * ((diff - min) / (max - min)) * ((diff - min) / (max - min)));

            mViewOffset.z -= fact;
            mLookAtOffset.y += 3 * fact;
            mViewOffset.y = 3.42 - ((3.42 - mViewOffset.y) * 1.2);
        }
    }

    //Python utick event (do it before camera handling to allow offset-modifications).
    NGF_PY_CALL_EVENT(utick, evt.timeSinceLastFrame);

    mCamNode->setFixedYawAxis(true, GlbVar.gravMgr->getSign() * Ogre::Vector3::UNIT_Y);
    //Do the camera-handling based on state.
    switch (mCurrState)
    {
        case CS_THIRDPERSON:
            {
                //Weird stuff here.
                if (!mTargetNode)
                    break;

                if (GlbVar.gravMgr->isUp())
                    mViewRoll = Ogre::Quaternion::Slerp(2 * evt.timeSinceLastFrame, mViewRoll, Ogre::Quaternion::IDENTITY, true);
                else
                    mViewRoll = Ogre::Quaternion::Slerp(2 * evt.timeSinceLastFrame, mViewRoll, 
                            Ogre::Quaternion(((Ogre::Math::UnitRandom() > 0.5) ? -1 : 1) * Ogre::Degree(179.8), Ogre::Vector3::NEGATIVE_UNIT_Z), 
                            true);

                mViewOffset = mViewRoll * mViewOffset;

                Ogre::Vector3 target = mTargetNode->getPosition() + (mTargetNode->getOrientation() * mViewOffset);
                Ogre::Real factor = mMovementFactor;
                Ogre::Vector3 lookAtOffset = mViewRoll * mLookAtOffset;

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
                                && (!(proxy0->m_collisionFilterGroup & DimensionManager::INVISIBLE)); //This is a visibility-based cast.
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
                Ogre::Vector3 toMove = (target - mCamNode->getPosition()) * factor * evt.timeSinceLastFrame;
                mCamNode->translate(toMove);

                //Gotta look at the guy. :P
                mCamNode->setFixedYawAxis(true, mViewRoll * mTargetNode->getOrientation() * Ogre::Vector3::UNIT_Y);
                lookAt(mTargetNode->getPosition() + lookAtOffset, evt.timeSinceLastFrame);
            }
            break;

        case CS_LOOKAT:
            if (mTargetNode)
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
                mCamNode->setPosition(mSplineNode->getPosition());
            }
            break;

        case CS_DEATH:
            {
                //Move it up, rotate it.
                mGhostOffset.y += GlbVar.gravMgr->getSign() * 4 * evt.timeSinceLastFrame;
                mGhostOffset = Ogre::Quaternion(Ogre::Degree(mGhostDirection ? -20 : 20) * evt.timeSinceLastFrame, Ogre::Vector3::UNIT_Y) * mGhostOffset;

                //Move the Camera toward it smoothly, make it look at the point.
                Ogre::Vector3 toMove = ((mGhostPos + mGhostOffset) - mCamNode->getPosition()) * 4 * evt.timeSinceLastFrame;
                mCamNode->translate(toMove);
                lookAt(mGhostPos + GlbVar.gravMgr->getSign() * mLookAtOffset, evt.timeSinceLastFrame);
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
                case CS_DEATH:
                    mGhostPos = mTargetNode ? mTargetNode->getPosition() : Ogre::Vector3::ZERO;
                    //mGhostOffset = mViewRoll * (mTargetNode->getOrientation() * Ogre::Vector3(0, mViewOffset.y + 1, 3));
                    mGhostOffset = mTargetNode->getOrientation() * Ogre::Vector3(0, mViewOffset.y + GlbVar.gravMgr->getSign(), 3);
                    mTargetNode = 0;
                    mGhostDirection = (Ogre::Math::UnitRandom() > 0.5);
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
                currOffset = mCamNode->getPosition() - mTargetNode->getPosition();
            else
                currOffset = 0;
            Ogre::Vector3 newPos = msg.getParam<Ogre::Vector3>(0) + currOffset;
            mCamNode->setPosition(newPos);
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
        NGF_PY_RETURN(mCamNode->getPosition());
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
        NGF::ID id = GlbVar.player->getID();
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

        mLastKeyFrameTime = 0;

        NGF_PY_RETURN();
    }
    NGF_PY_METHOD_IMPL(addSplinePointAbsolute)
    {
        Ogre::Real time = py::extract<Ogre::Real>(args[0]);
        mLastKeyFrameTime += time;
        Ogre::TransformKeyFrame *key = mSplineTrack->createNodeKeyFrame(time);
        key->setTranslate(py::extract<Ogre::Vector3>(args[1]));

        NGF_PY_RETURN();
    }
    NGF_PY_METHOD_IMPL(addSplinePoint)
    {
        mLastKeyFrameTime += py::extract<Ogre::Real>(args[0]);
        Ogre::TransformKeyFrame *key = mSplineTrack->createNodeKeyFrame(mLastKeyFrameTime);
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
