/*
 * =====================================================================================
 *
 *       Filename:  CameraHandler.h
 *
 *    Description:  A GameObject interface to Camera handling. Allows third-person
 *                  views, spline paths etc.
 *
 *        Created:  05/12/2009 03:19:31 PM
 *       Compiler:  gcc
 *
 *         Author:  Nikhilesh (nikki)
 *
 * =====================================================================================
 */

#ifndef __CAMERAHANDLER_H__
#define __CAMERAHANDLER_H__

#include "Globals.h"

class CameraHandler :
    public NGF::Python::PythonGameObject,
    public NGF::Serialisation::SerialisableGameObject
{
    public:
        enum
        {
            CS_NONE,
            CS_LOOKAT,
            CS_THIRDPERSON,
            CS_SPLINE,
        };

    protected:
        Ogre::Camera *mCamera;
        Ogre::SceneNode *mCameraNode;

        Ogre::SceneNode *mTargetNode;
        Ogre::Vector3 mOffset;
        Ogre::Real mSmoothingFactor; //The fraction of the required displacement that the camera moves by.

        int mCurrState;

    public:
        CameraHandler(Ogre::Vector3 pos, Ogre::Quaternion rot, NGF::ID id, NGF::PropertyList properties, Ogre::String name);
        virtual ~CameraHandler();

        //--- Events -------------------------------------------------------------------
        void unpausedTick(const Ogre::FrameEvent &evt);
        void pausedTick(const Ogre::FrameEvent &evt);
        NGF::MessageReply receiveMessage(NGF::Message msg); 

        //--- Non-NGF ------------------------------------------------------------------
        inline void lookAt(Ogre::Vector3 target)
        {
            //mCameraNode->lookAt(target, Ogre::Node::TS_PARENT);
            
            Ogre::Vector3 dir = target - mCameraNode->getPosition();
            //dir.y = 0;
            dir.normalise();

            Ogre::Vector3 src = mCameraNode->getOrientation() * Ogre::Vector3::NEGATIVE_UNIT_Z;
            //src.y = 0;
            src.normalise();

            Ogre::Quaternion rot = src.getRotationTo(dir, Ogre::Vector3::UNIT_Y);
            mCameraNode->rotate(rot);
        }

        //--- Serialisation ------------------------------------------------------------
        NGF_SERIALISE_BEGIN(CameraHandler)
        {
            //For non-standard stuff.
            Ogre::String targetName;

            //While saving, store stuff.
            NGF_SERIALISE_ON_SAVE
            {
                targetName = mTargetNode->getName();
            }

            //The actual read/write.
            NGF_SERIALISE_POSITION(mCameraNode->getPosition());
            NGF_SERIALISE_ROTATION(mCameraNode->getPosition());

            NGF_SERIALISE_STRING(targetName);

            //While loading, get stuff.
            NGF_SERIALISE_ON_LOAD
            {
                mTargetNode = GlbVar.ogreSmgr->getSceneNode(targetName);
            }
        }
        NGF_SERIALISE_END
};

#ifdef __CAMERAHANDLER_CPP__

//gperf stuff goes here

#endif

#endif
