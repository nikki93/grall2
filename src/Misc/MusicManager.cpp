/*
=========================================
MusicManager.cpp
=========================================
*/

#include "Misc/MusicManager.h"

void MusicManager::tick(const Ogre::FrameEvent &evt)
{
    if (mPreviousSound)
    {
        mPreviousSound->setGain(clamp(mPreviousSound->getGain() - mFadeRate * evt.timeSinceLastFrame, 0.0f, mActualGain));

        if (mPreviousSound->getGain() == 0)
        {
            mPreviousSound->stop();
            mPreviousSound->setGain(mActualGain);
            mPreviousSound = 0;
        }
    }

    if (mCurrentSound && mCurrentSound->getGain() < mActualGain)
        mCurrentSound->setGain(clamp(mCurrentSound->getGain() + mFadeRate * evt.timeSinceLastFrame, 0.0f, mActualGain));
}
