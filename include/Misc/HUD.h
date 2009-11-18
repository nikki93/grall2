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
 * The HUD class manages the HUD. Simple interface calls such as 'addTimer' and
 * 'addIcon' are all that's needed, the positioning etc. is managed automatically
 */

class HUD
{
    protected:
        struct Timer
        {
            Ogre::Real mTime;
            MyGUI::StaticTextPtr mText;

            Timer(Ogre::Real time, Ogre::Vector2 pos, const Ogre::ColourValue &colour);
            ~Timer();
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
            Ogre::String mTextureName;

            Icon(const Ogre::String &imageFile);
            ~Icon();
            void update(const Ogre::Vector2 &pos);
            void setImage(const Ogre::String &imageFile);
        };

        typedef std::map<int, Timer *> TimerMap;
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

    protected:

        //--- For serialisation --------------------------------------------------------

        friend class Controller;

        struct TimerRecord
        {
            Ogre::Real time; MyGUI::Colour colour;

            TimerRecord() {}
            TimerRecord(Ogre::Real t, const MyGUI::Colour &c)
                : time(t), colour(c)
            {
            }

            template<class Archive>
            void serialize(Archive &ar, const unsigned int version)
            {
                ar & time;
                ar & colour.red;
                ar & colour.green;
                ar & colour.blue;
                ar & colour.alpha;
            }
        };
        typedef std::vector<TimerRecord> TimerRecordVec;

        struct PickupDisplayRecord
        {
            MyGUI::Colour colour;

            PickupDisplayRecord() {}
            PickupDisplayRecord(const MyGUI::Colour &c)
                : colour(c)
            {
            }

            template<class Archive>
            void serialize(Archive &ar, const unsigned int version)
            {
                ar & colour.red;
                ar & colour.green;
                ar & colour.blue;
                ar & colour.alpha;
            }
        };
        typedef std::map<Ogre::String, PickupDisplayRecord> PickupDisplayRecordMap;

        struct IconRecord
        {
            Ogre::String textureName;

            IconRecord() {}
            IconRecord(const Ogre::String &t)
                : textureName(t)
            {
            }

            template<class Archive>
            void serialize(Archive &ar, const unsigned int version)
            {
                ar & textureName;
            }
        };
        typedef std::map<Ogre::String, IconRecord> IconRecordMap;

        void writeTimerRecords(TimerRecordVec &out)
        {
            for (TimerMap::iterator iter = mTimers.begin();
                    iter != mTimers.end(); ++iter)
            {
                Timer *timer = iter->second;
                out.push_back(TimerRecord(timer->mTime, timer->mText->getTextColour()));
            }
        }

        void writePickupDisplayRecords(PickupDisplayRecordMap &out)
        {
            for (PickupDisplayMap::iterator iter = mPickupDisplays.begin();
                    iter != mPickupDisplays.end(); ++iter)
            {
                PickupDisplay *pd = iter->second;
                out.insert(std::make_pair(pd->mType, pd->mText->getTextColour()));
            }
        }

        void writeIconRecords(IconRecordMap &out)
        {
            for (IconMap::iterator iter = mIcons.begin();
                    iter != mIcons.end(); ++iter)
            {
                out.insert(std::make_pair(iter->first, iter->second->mTextureName));
            }
        }
};

#endif
