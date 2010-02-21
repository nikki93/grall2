/*
=========================================
MusicManager.cpp
=========================================
*/

#include "Misc/MusicManager.h"

/*
void MusicManager::tick(const Ogre::FrameEvent &evt)
{
    if (mPreviousSound)
    {
        mPreviousSound->setGain(Util::clamp(mPreviousSound->getGain() - mFadeRate * evt.timeSinceLastFrame, 0.0f, mActualGain));

        if (mPreviousSound->getGain() == 0)
        {
            mPreviousSound->stop();
            mPreviousSound->setGain(mActualGain);
            mPreviousSound = 0;
        }
    }

    if (mCurrentSound && mCurrentSound->getGain() < mActualGain)
        mCurrentSound->setGain(Util::clamp(mCurrentSound->getGain() + mFadeRate * evt.timeSinceLastFrame, 0.0f, mActualGain));
}
*/

void MusicManager::tick(const Ogre::FrameEvent &evt)
{
    if (mFadeOutTrack && mFadeOutTrack->getGain() > 0)
    {
        Ogre::Real newGain = mFadeOutTrack->getGain() - 0.3 * evt.timeSinceLastFrame;

        if (newGain > 0)
            mFadeOutTrack->setGain(newGain);
        else
        {
            if (mCurrGroup != mFadeOutTrack->getUserAny().operator()<Group*>())
                stopInGroup(mFadeOutTrack);
            mFadeOutTrack->setGain(0);
            mFadeOutTrack = 0;
        }
    }

    if (mCurrTrack && mCurrTrack->getGain() < mActualGain)
        mCurrTrack->setGain(Util::clamp(mCurrTrack->getGain() + 0.3f * evt.timeSinceLastFrame, 0.0f, mActualGain));
}
