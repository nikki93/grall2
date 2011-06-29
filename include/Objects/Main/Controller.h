/*
 * =====================================================================================
 *
 *       Filename:  Controller.h
 *
 *    Description:  Controller GameObject. He's always around, and watching over things.
 *                  ;-) He takes care of serialisation of stuff that doesn't really
 *                  belong to any object. Takes care of 'level' stuff, like bonus time
 *                  etc. Also handles serialisation of global level properties.
 *
 *        Created:  07/23/2009 02:36:12 PM
 *       Compiler:  gcc
 *
 *         Author:  Nikhilesh (nikki)
 *
 * =====================================================================================
 */

#ifndef __CONTROLLER_H__
#define __CONTROLLER_H__

#include "Globals.h"

#include <boost/serialization/map.hpp>
#include <boost/serialization/vector.hpp>

class Controller :
    public NGF::Serialisation::SerialisableGameObject,
    public NGF::Python::PythonGameObject,
    public AlarmGameObject,
    public Util::ExtraEventListener
{
    protected:
        Ogre::Real mEndCountDown;
        bool mWin;

        MyGUI::StaticTextPtr mLevelText;
        Ogre::Real mLevelTextRate;
        Ogre::Real mLevelTextPause;
        Ogre::Real mLevelTextOut;

    public:
        Controller(Ogre::Vector3 pos, Ogre::Quaternion rot, NGF::ID id, NGF::PropertyList properties, Ogre::String name);
        ~Controller();

        //--- Events -------------------------------------------------------------------
        void postLoad();
        void unpausedTick(const Ogre::FrameEvent &evt);
        void pausedTick(const Ogre::FrameEvent &evt);
        void alarm(Alarm a) { NGF_PY_CALL_EVENT(alarm, a); }
        NGF::MessageReply receiveMessage(NGF::Message msg);

        //--- Python interface ---------------------------------------------------------
        NGF_PY_BEGIN_DECL(Controller)
        {
            NGF_PY_METHOD_DECL(setAlarm)
            NGF_PY_METHOD_DECL(showLevelText)
        }
        NGF_PY_END_DECL

        //--- Serialisation ------------------------------------------------------------
        NGF_SERIALISE_BEGIN(Controller)
        {
            GRALL2_SERIALISE_ALARMS();
            NGF_SERIALISE_PYTHON_LOCALS();

            // ---
            
            int dimension, bonusTime;
            bool gravity;
            Ogre::String musicMood, musicGroup;

            HUD::TimerRecordVec hudTimers;
            HUD::PickupDisplayRecordMap hudPickups;
            HUD::IconRecordMap hudIcons;

            //--- On save --------------------------------------------------------------

            NGF_SERIALISE_ON_SAVE
            {
                //Dimension.
                dimension = GlbVar.dimMgr->getCurrentDimension();

                //Gravity.
                gravity = GlbVar.gravMgr->isUp();

                //Music.
                musicGroup = GlbVar.musicMgr->getCurrentGroupName();
                musicMood = GlbVar.musicMgr->getMood();

                //Bonus time.
                bonusTime = GlbVar.bonusTime;

                //HUD.
                GlbVar.hud->writeTimerRecords(hudTimers);
                GlbVar.hud->writePickupDisplayRecords(hudPickups);
                GlbVar.hud->writeIconRecords(hudIcons);
            }

            //--- Read/write -----------------------------------------------------------

            //Dimension.
            NGF_SERIALISE_OGRE(Int, dimension);

            //Gravity.
            NGF_SERIALISE_OGRE(Bool, gravity);

            //Music.
            NGF_SERIALISE_STRING(musicGroup);
            NGF_SERIALISE_STRING(musicMood);

            //Bonus time.
            NGF_SERIALISE_OGRE(Int, bonusTime);

            //HUD.
            NGF_SERIALISE_STL_CONTAINER(hudTimers);
            NGF_SERIALISE_STL_CONTAINER(hudPickups);
            NGF_SERIALISE_STL_CONTAINER(hudIcons);

            //--- On load --------------------------------------------------------------

            NGF_SERIALISE_ON_LOAD
            {
                //Dimension.
                GlbVar.dimMgr->setDimension(dimension);

                //Gravity.
                GlbVar.gravMgr->setUp(gravity);

                //Music.
                if (musicGroup != "none")
                {
                    GlbVar.musicMgr->play(musicGroup);
                    GlbVar.musicMgr->setMood(musicMood);
                }
                else
                    GlbVar.musicMgr->stop();

                //Bonus time.
                GlbVar.bonusTime = bonusTime;

                //HUD.
                GlbVar.hud->clear();

                for (HUD::TimerRecordVec::iterator iter = hudTimers.begin();
                        iter != hudTimers.end(); ++iter)
                    GlbVar.hud->addTimer(iter->time, Ogre::ColourValue(
                                iter->colour.red,
                                iter->colour.green,
                                iter->colour.blue,
                                iter->colour.alpha
                                ));

                for (HUD::PickupDisplayRecordMap::iterator iter = hudPickups.begin();
                        iter != hudPickups.end(); ++iter)
                    GlbVar.hud->addPickupDisplay(iter->first, Ogre::ColourValue(
                                iter->second.colour.red,
                                iter->second.colour.green,
                                iter->second.colour.blue,
                                iter->second.colour.alpha
                                ));

                for (HUD::IconRecordMap::iterator iter = hudIcons.begin();
                        iter != hudIcons.end(); ++iter)
                    GlbVar.hud->setIcon(iter->first, iter->second.textureName);
            }
        }
        NGF_SERIALISE_END
};

#ifdef __CONTROLLER_CPP__

/* C++ code produced by gperf version 3.0.4 *//*{{{*/
/* Command-line: gperf  */
/* Computed positions: -k'' */


#ifndef __PYTHON_METHOD_STRUCT__
#define __PYTHON_METHOD_STRUCT__
struct PythonMethod
{
const char *name;
int code;
};
#endif //;
/* maximum key range = 6, duplicates = 0 */

class NGF_PY_CLASS_GPERF(Controller)
{
private:
  static inline unsigned int MakeHash (const char *str, unsigned int len);
public:
  static const struct PythonMethod *Lookup (const char *str, unsigned int len);
};

inline /*ARGSUSED*/
unsigned int
NGF_PY_CLASS_GPERF(Controller)::MakeHash (register const char *str, register unsigned int len)
{
  return len;
}

const struct PythonMethod *
NGF_PY_CLASS_GPERF(Controller)::Lookup (register const char *str, register unsigned int len)
{
  enum
    {
      TOTAL_KEYWORDS = 2,
      MIN_WORD_LENGTH = 8,
      MAX_WORD_LENGTH = 13,
      MIN_HASH_VALUE = 8,
      MAX_HASH_VALUE = 13
    };

  static const struct PythonMethod wordlist[] =
    {
      {"setAlarm", NGF_PY_METHOD_GPERF(Controller, setAlarm)},
      {"showLevelText", NGF_PY_METHOD_GPERF(Controller, showLevelText)}
    };

  if (len <= MAX_WORD_LENGTH && len >= MIN_WORD_LENGTH)
    {
      register int key = MakeHash (str, len);

      if (key <= MAX_HASH_VALUE && key >= MIN_HASH_VALUE)
        {
          register const struct PythonMethod *resword;

          switch (key - 8)
            {
              case 0:
                resword = &wordlist[0];
                goto compare;
              case 5:
                resword = &wordlist[1];
                goto compare;
            }
          return 0;
        compare:
          {
            register const char *s = resword->name;

            if (*str == *s && !strcmp (str + 1, s + 1))
              return resword;
          }
        }
    }
  return 0;
}/*}}}*/

#endif

#endif

