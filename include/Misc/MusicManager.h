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

#define LOAD_MUSIC(music) GlbVar.soundMgr->createSound(#music, #music ".ogg", true, true)

#include "Globals.h"

class MusicManager
{
    protected:
        typedef Ogre::String Mood;
        typedef std::map<Mood, OgreAL::Sound*> Group;
        typedef std::map<Ogre::String, Group> GroupMap;

        Ogre::String mMood;
        GroupMap mGroups;
        Ogre::Real mActualGain;

        OgreAL::Sound *mFadeOutTrack;
        OgreAL::Sound *mCurrTrack;
        Group *mCurrGroup;
        Ogre::String mCurrGroupName;

        void stopInGroup(OgreAL::Sound *track)
        {
            Group *group = track->getUserAny().operator()<Group*>(); // >_> ...

            for (Group::iterator iter = group->begin(); iter != group->end(); ++iter)
                iter->second->stop();
        }

        void playInGroup(OgreAL::Sound *track)
        {
            Group *group = track->getUserAny().operator()<Group*>(); // >_> ...

            for (Group::iterator iter = group->begin(); iter != group->end(); ++iter)
            {
                iter->second->play();

                if (iter->second == track)
                    track->setGain(mActualGain);
                else
                    iter->second->setGain(0);
            }
        }

    public:
        MusicManager()
            : mMood("calm"),
              mCurrTrack(0),
              mFadeOutTrack(0),
              mCurrGroup(0),
              mActualGain(1),
              mCurrGroupName("none")
        {
            Ogre::ConfigFile cfg;
            cfg.loadFromResourceSystem("Music.ini", "General");

            Ogre::ConfigFile::SectionIterator iter = cfg.getSectionIterator();

            for (; iter.hasMoreElements(); iter.moveNext())
            {
                Ogre::String group = iter.peekNextKey();

                Ogre::ConfigFile::SettingsMultiMap *moods = iter.peekNextValue();

                for (Ogre::ConfigFile::SettingsMultiMap::iterator iter = moods->begin();
                        iter != moods->end(); ++iter)
                {
                    mGroups[group][iter->first] = GlbVar.soundMgr->createSound(iter->second, iter->second, true, true);
                    mGroups[group][iter->first]->setUserAny(Ogre::Any(&(mGroups[group])));
                }
            }
        }
        
        void tick(const Ogre::FrameEvent &evt);

        void fadeIn(const Ogre::String &groupName)
        {
            mFadeOutTrack = mCurrTrack;

            Group &group = mGroups[groupName];
            mCurrGroup = &group;
            mCurrGroupName = groupName;
            Group::iterator iter = group.find(mMood);

            if (iter == group.end())
                mCurrTrack = group.begin()->second;
            else
                mCurrTrack = iter->second;

            playInGroup(mCurrTrack);
            mCurrTrack->setGain(0);
        }

        void fadeOut()
        {
            mFadeOutTrack = mCurrTrack;
            mCurrTrack = 0;
            mCurrGroup = 0;
            mCurrGroupName = "none";
        }

        void play(const Ogre::String &groupName)
        {
            if (groupName == mCurrGroupName)
                return;

            if (mCurrTrack)
                stopInGroup(mCurrTrack);

            if (mFadeOutTrack)
            {
                mFadeOutTrack->stop();
                mFadeOutTrack = 0;
            }

            Group &group = mGroups[groupName];
            mCurrGroup = &group;
            mCurrGroupName = groupName;
            Group::iterator iter = group.find(mMood);

            if (iter == group.end())
                mCurrTrack = group.begin()->second;
            else
                mCurrTrack = iter->second;

            playInGroup(mCurrTrack);
        }

        Ogre::String getCurrentGroupName()
        {
            return mCurrGroupName;
        }

        void stop()
        {
            if (mCurrTrack)
            {
                stopInGroup(mCurrTrack);
                mCurrTrack = 0;
                mCurrGroup = 0;
                mCurrGroupName = "none";
            }
        }

        void switchMood(const Mood &mood)
        {
            if (mCurrTrack)
                mCurrTrack->setGain(0);

            mMood = mood;

            if (mCurrGroup)
            {
                Group &group = *mCurrGroup;
                Group::iterator iter = group.find(mMood);

                if (iter == group.end())
                    mCurrTrack = group.begin()->second;
                else
                    mCurrTrack = iter->second;

                mCurrTrack->setGain(mActualGain);
                mCurrTrack->play();
            }
        }

        void setMood(const Mood &mood)
        {
            if (mMood == mood)
                return;

            mFadeOutTrack = mCurrTrack;
            mMood = mood;

            if (mCurrGroup)
            {
                Group &group = *mCurrGroup;
                Group::iterator iter = group.find(mMood);

                if (iter == group.end())
                    mCurrTrack = group.begin()->second;
                else
                    mCurrTrack = iter->second;

                mCurrTrack->setGain(0);
                mCurrTrack->play();
            }
        }

        Ogre::String getMood()
        {
            return mMood;
        }

        void setGain(Ogre::Real gain)
        {
            mActualGain = gain;
            if (mCurrTrack)
                mCurrTrack->setGain(gain);
        }

        Ogre::Real getGain()
        {
            return mActualGain;
        }
};

#endif
