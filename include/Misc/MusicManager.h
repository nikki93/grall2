/*
 * =====================================================================================
 *
 *       Filename:  MusicManager.h
 *
 *    Description:  Our very own little JukeBox. :-)
 *
 *        Created:  07/05/2009 11:01:34 PM
 *       Compiler:  gcc
 *
 *         Author:  Nikhilesh (nikki)
 *
 * =====================================================================================
 */

#ifndef __MUSIC_MANAGER_H__
#define __MUSIC_MANAGER_H__

#define LOAD_MUSIC(music) GlbVar.soundMgr->createSound(#music, #music ".ogg", true, false)

#include "Globals.h"

class MusicManager
{
    protected:
        OgreAL::Sound *mPreviousSound;
        OgreAL::Sound *mCurrentSound;
        Ogre::Real mFadeRate;

        Ogre::Real mActualGain;

    public:
        MusicManager()
            : mCurrentSound(0),
              mPreviousSound(0),
              mActualGain(1)
        {
            LOAD_MUSIC(Track1);
            LOAD_MUSIC(Track2);
        }

        ~MusicManager() 
        {
        }

        void playMusic(Ogre::String music)
        {
            //Don't cut if same song.
            if (mCurrentSound)
            {
                if (music == mCurrentSound->getName())
                    return;
                mCurrentSound->stop();
            }
            mCurrentSound = GlbVar.soundMgr->getSound(music);
            mCurrentSound->play();
            mCurrentSound->setGain(mActualGain);
        }

        void crossFade(Ogre::String music, Ogre::Real time)
        {
            //Don't fade to same song.
            if (mCurrentSound && music == mCurrentSound->getName())
                return;

            mPreviousSound = mCurrentSound;
            mCurrentSound = GlbVar.soundMgr->getSound(music);
            mCurrentSound->stop();
            mCurrentSound->play();
            mCurrentSound->setGain(0);

            mFadeRate = mActualGain / time;
        }

        void setGain(Ogre::Real gain)
        {
            mCurrentSound->setGain(gain);
            mActualGain = gain;
        }

        Ogre::Real getGain()
        {
            return mActualGain;
        }

        void tick(const Ogre::FrameEvent &evt);
};

#endif
