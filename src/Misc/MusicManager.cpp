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
        Ogre::Real newGain = mFadeOutTrack->getGain() - 0.2 * evt.timeSinceLastFrame;

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
        mCurrTrack->setGain(Util::clamp(mCurrTrack->getGain() + 0.2f * evt.timeSinceLastFrame, 0.0f, mActualGain));

    /*
    LOG(FORMAT("track1:\n  calm - playing: %1%, gain: %2%\n  intense - playing: %3%, gain: %4%",
                mGroups["track1"]["calm"]->isPlaying() %
                mGroups["track1"]["calm"]->getGain() %
                mGroups["track1"]["intense"]->isPlaying() %
                mGroups["track1"]["intense"]->getGain()));
    */
}
