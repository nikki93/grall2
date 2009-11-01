/*
 * =====================================================================================
 *
 *       Filename:  HUD.h
 *
 *    Description:  Manages the HUD.
 *
 *        Created:  11/01/2009 04:00:46 PM
 *       Compiler:  gcc
 *
 *         Author:  Nikhilesh S (nikki)
 *
 * =====================================================================================
 */

#ifndef __HUD_H__
#define __HUD_H__

#include "Globals.h"

/*
 * The HUD class manages the HUD. Anyone can call 'addTimer' to add a timer, and the HUD
 * will dynamically update and arrange this timers on screen. It also shows bonus time,
 * which is given a special location among the other timers.
 */

class HUD
{
    protected:
        struct HUDTimer
        {
            Ogre::Real mTime;
            MyGUI::StaticTextPtr mText;

            HUDTimer(Ogre::Real time, Ogre::Vector2 pos, const Ogre::ColourValue &colour);
            ~HUDTimer();
            bool update(Ogre::Real elapsed);
        };
        typedef std::map<int, HUDTimer *> TimerMap;

        TimerMap mTimers;

        MyGUI::StaticTextPtr mBonusTimer;

    public:
        HUD();
        ~HUD()
        {
            clear();

            //Remove bonus timer.
            GlbVar.gui->destroyWidget(mBonusTimer);
        }

        void clear();
        void tick(const Ogre::FrameEvent &evt);

        //Returns the 'ID' of the timer in the internal list.
        int addTimer(Ogre::Real time, const Ogre::ColourValue &colour = Ogre::ColourValue::White);
        void removeTimer(int timer);
};

#endif
