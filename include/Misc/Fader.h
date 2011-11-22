/*
 * =====================================================================================
 *
 *       Filename:  Fader.h
 *
 *    Description:  Handles screen fading effects.
 *
 *        Created:  06/22/2009 07:28:37 PM
 *       Compiler:  gcc
 *
 *         Author:  Nikhilesh (nikki)
 *
 * =====================================================================================
 */

#ifndef __FADER_H__
#define __FADER_H__

#include "Globals.h"

#include <MyGUI_TileRect.h>

class Fader
{
    protected: 
        MyGUI::TextBox *mImage;

        enum
        {
            FM_NONE,
            FM_IN,
            FM_OUT,
            FM_INOUT
        };

        int mState;
        Ogre::Real mRate;
        Ogre::Real mPause;
        Ogre::Real mFadeInOutOutTime; //Crazy name, I know.

    public:
        Fader()
            : mFadeInOutOutTime(0)
        {
            int winHeight = GlbVar.ogreWindow->getHeight();
            int winWidth = GlbVar.ogreWindow->getWidth();
            mImage = GlbVar.gui->createWidget<MyGUI::TextBox>("ClientTileSkin", MyGUI::IntCoord(0,0,winWidth,winHeight), MyGUI::Align::Default, "Fade");
            mImage->setAlpha(0);

            mState = FM_NONE;
        }

        ~Fader()
        {
            GlbVar.gui->destroyWidget(mImage);
        }

        void fadeIn(const Ogre::ColourValue &colour, Ogre::Real time)
        {
            setColour(colour);
            mImage->setAlpha(0);
            mState = FM_IN;
            mRate = 1 / time;
        }

        void fadeOut(const Ogre::ColourValue &colour, Ogre::Real time)
        {
            setColour(colour);
            mImage->setAlpha(1);
            mState = FM_OUT;
            mRate = 1 / time;
        }

        inline void setColour(const Ogre::ColourValue &colour)
        {
            MyGUI::Colour col(colour.r, colour.g, colour.b, colour.a);
            MyGUI::TileRect *raw = mImage->getSubWidgetMain()->castType<MyGUI::TileRect>();
            raw->_setColour(col);
        }

        //Abort the fading, and jump to given alpha.
        void abortFade(Ogre::Real alpha)
        {
            mImage->setAlpha(alpha);
            mState = FM_NONE;
        }

        //'time' is the time for each step (fading in, and fading out).
        //'pause' is the time between each step.
        void fadeInOut(const Ogre::ColourValue &colour, Ogre::Real in, Ogre::Real pause, Ogre::Real out = -1)
        {
            setColour(colour);
            mPause = pause;
            mImage->setAlpha(0);
            mState = FM_INOUT;
            mRate = 1 / in;
            mFadeInOutOutTime = out == -1 ? in : out;
        }

        void tick(const Ogre::FrameEvent &evt);
};

#endif
