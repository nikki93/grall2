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

        struct PickupDisplay
        {
            Ogre::String mType;
            MyGUI::StaticTextPtr mText;

            PickupDisplay(const Ogre::String &type, const Ogre::ColourValue &colour);
            ~PickupDisplay();
            void update(const Ogre::Vector2 &pos);
        };

        struct Icon
        {
            MyGUI::StaticImagePtr mImage;

            Icon(const Ogre::String &imageFile);
            ~Icon();
            void update(const Ogre::Vector2 &pos);
            void setImage(const Ogre::String &imageFile);
        };

        typedef std::map<int, HUDTimer *> TimerMap;
        TimerMap mTimers;

        typedef std::map<Ogre::String, PickupDisplay *> PickupDisplayMap;
        PickupDisplayMap mPickupDisplays;

        typedef std::map<Ogre::String, Icon *> IconMap;
        IconMap mIcons;

        MyGUI::StaticTextPtr mBonusTimer;

    public:
        HUD();
        ~HUD()
        {
            clear();

            //Remove bonus timer.
            GlbVar.gui->destroyWidget(mBonusTimer);
        }

        void tick(const Ogre::FrameEvent &evt);
        void clear();

        //Handle bonus time display.
        inline void setBonusTimerVisible(bool visible) { if (mBonusTimer) mBonusTimer->setVisible(visible); }

        //Adds a timer, returns the 'ID' of the timer in the internal list.
        int addTimer(Ogre::Real time, const Ogre::ColourValue &colour = Ogre::ColourValue::White);
        void removeTimer(int timer);

        //Adds a pickup display (tracks given pickup), with given colour.
        void addPickupDisplay(const Ogre::String &type, const Ogre::ColourValue &colour = Ogre::ColourValue::White);
        void removePickupDisplay(const Ogre::String &type);

        //Adds icon identified by given name, with given image. If icon already exists, updates the image.
        void setIcon(const Ogre::String &name, const Ogre::String &imageFile);
        void removeIcon(const Ogre::String &name);
};

#endif
