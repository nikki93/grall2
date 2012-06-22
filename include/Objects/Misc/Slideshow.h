/*
 * =====================================================================================
 *
 *       Filename:  Slideshow.h
 *
 *    Description:  Plays a slideshow, allowing different times for each frame. Great
 *                  for 'storytelling'.
 *
 *        Created:  10/04/2009 05:06:31 PM
 *       Compiler:  gcc
 *
 *         Author:  Nikhilesh S (nikki)
 *
 * =====================================================================================
 */

#ifndef __SLIDESHOW_H__
#define __SLIDESHOW_H__

#include "Globals.h"

class Slideshow :
    public AlarmGameObject,
    public NGF::Serialisation::SerialisableGameObject
{
    protected:
        MyGUI::StaticImagePtr mBackground;
        MyGUI::StaticImagePtr mImage;

        std::vector<Ogre::String> mSlides; //stores slide 2 onward
        int mCurrInd;
        Ogre::Real mCurrTime;

        int mImgWidth;
        int mImgHeight;

    public:
        Slideshow(Ogre::Vector3 pos, Ogre::Quaternion rot, NGF::ID id, NGF::PropertyList properties, Ogre::String name);
        ~Slideshow();

        //--- Events -------------------------------------------------------------------
        void postLoad();
        void unpausedTick(const Ogre::FrameEvent &evt);
        void pausedTick(const Ogre::FrameEvent &evt);
        void alarm(Alarm a);
        NGF::MessageReply receiveMessage(NGF::Message msg);

        //--- Non-NGF ------------------------------------------------------------------
        void updateCoords();

        //--- Serialisation ------------------------------------------------------------
        NGF_SERIALISE_BEGIN(Slideshow)
        {
            GRALL2_SERIALISE_ALARMS();
        }
        NGF_SERIALISE_END
};

#ifdef __SLIDESHOW_CPP__


#endif

#endif
