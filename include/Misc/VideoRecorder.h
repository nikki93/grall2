/*
 * =====================================================================================
 *
 *       Filename:  VideoRecorder.h
 *
 *    Description:  A primitive video recorder. Records by taking screenshots every 
 *                  fraction of a second, saving them as seperate files, which can then
 *                  be stitched together using your favourite video-stuff program. :-)
 *
 *        Created:  07/09/2009 05:01:35 PM
 *       Compiler:  gcc
 *
 *         Author:  Nikhilesh (nikki)
 *
 * =====================================================================================
 */

#ifndef __VIDEORECORDER_H__
#define __VIDEORECORDER_H__

#include "Globals.h"

#define VIDEO_FORMAT ".jpg"

class VideoRecorder
{
    protected:
        long int mCount;
        Ogre::Real mTimeLeft;
        Ogre::Real mPeriod;

        bool mRecording;

    public:
        VideoRecorder()
            : mCount(0),
              mTimeLeft(0),
              mPeriod(0),
              mRecording(false)
        {
        }

        void startRecording(Ogre::Real period)
        {
            mTimeLeft = (mPeriod = period);
            mRecording = true;
        }

        void stopRecording()
        {
            mRecording = false;
        }

        void tick(const Ogre::FrameEvent &evt)
        {
            if (mRecording)
            {
                if (mTimeLeft < 0)
                {
                    GlbVar.ogreWindow->writeContentsToFile(USER_PREFIX + "Video/frame" + Ogre::StringConverter::toString(mCount++) + VIDEO_FORMAT);
                    mTimeLeft = mPeriod;
                }
                mTimeLeft -= evt.timeSinceLastFrame;
            }
        }
};

#endif
