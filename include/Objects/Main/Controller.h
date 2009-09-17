/*
 * =====================================================================================
 *
 *       Filename:  Controller.h
 *
 *    Description:  Controller GameObject. He's always around, and watching over things.
 *                  ;-) He takes care of serialisation of stuff that doesn't really
 *                  belong to any object. Can also be used for non-GameObject-specific
 *                  Python scripting.
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
        void pausedTick(const Ogre::FrameEvent &evt) {}
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
            //Save dimension, music.
            int dimension, bonusTime;
            Ogre::String musicName;

            NGF_SERIALISE_ON_SAVE
            {
                dimension = GlbVar.dimMgr->getCurrentDimension();
                musicName = GlbVar.musicMgr->getCurrentMusic();
                bonusTime = GlbVar.bonusTime;
            }

            NGF_SERIALISE_OGRE(Int, dimension);
            NGF_SERIALISE_STRING(musicName);
            NGF_SERIALISE_OGRE(Int, bonusTime); //Save bonus time left.

            GRALL2_SERIALISE_ALARMS();

            NGF_SERIALISE_ON_LOAD
            {
                GlbVar.dimMgr->setDimension(dimension);

                if (musicName != "<none>")
                    GlbVar.musicMgr->playMusic(musicName);
                else
                    GlbVar.musicMgr->stopMusic();

                GlbVar.bonusTime = bonusTime;
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

