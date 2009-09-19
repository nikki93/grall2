/*
 * =====================================================================================
 *
 *       Filename:  Turret.h
 *
 *    Description:  The Turret GameObject. Annoys the Player for a while and then goes
 *                  into hibernation. Repeats. :P
 *
 *        Version:  1.0
 *        Created:  09/18/2009 12:31:23 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Dr. Fritz Mehner (mn), mehner@fh-swf.de
 *        Company:  FH Südwestfalen, Iserlohn
 *
 * =====================================================================================
 */

#ifndef __TURRET_H__
#define __TURRET_H__

#include "Globals.h"
#include "Objects/Main/GraLL2GameObject.h"

class Turret :
    public GraLL2GameObject
{
    protected:
        btCollisionShape *mShape;

        Ogre::SceneNode *mTopNode;
        Ogre::SceneNode *mBaseNode;
        Ogre::Entity *mTopEntity;
        Ogre::Entity *mBaseEntity;

        Ogre::Real mRadius;

        Ogre::Real mFireTime;
        Ogre::Real mRestTime;

        enum
        {
            TS_FIRETOREST,
            TS_REST,
            TS_RESTTOFIRE,
            TS_FIRE,
            TS_SCAN,
        };

        int mState;

        Ogre::Real mStateTimer;
        Ogre::Real mBulletTimer;

        bool mAlwaysScan;

    public:
        Turret(Ogre::Vector3 pos, Ogre::Quaternion rot, NGF::ID id, NGF::PropertyList properties, Ogre::String name);
        virtual ~Turret();

        //--- Events -------------------------------------------------------------------
        void postLoad();
        void unpausedTick(const Ogre::FrameEvent &evt);
        void pausedTick(const Ogre::FrameEvent &evt);
        NGF::MessageReply receiveMessage(NGF::Message msg);
        void collide(GameObject *other, btCollisionObject *otherPhysicsObject, btManifoldPoint &contact);

        //--- Non-NGF ------------------------------------------------------------------
        void startFiring();
        void stopFiring();
        void fire(Ogre::Real time);
        void rest(Ogre::Real time);
        void scan();
        void fireSingleBullet();
        bool doSingleScan();
        bool playerIsInRadius();

        //--- Python interface ---------------------------------------------------------
        NGF_PY_BEGIN_DECL(Turret)
        {
            NGF_PY_METHOD_DECL(startFiring)
            NGF_PY_METHOD_DECL(stopFiring)

            NGF_PY_PROPERTY_DECL(radius)
            NGF_PY_PROPERTY_DECL(fireTime)
            NGF_PY_PROPERTY_DECL(restTime)
            NGF_PY_PROPERTY_DECL(alwaysScan)
        }
        NGF_PY_END_DECL

        //--- Serialisation ------------------------------------------------------------
        NGF_SERIALISE_BEGIN(Turret)
        {
            //Base's position is always same (0,0,0), no need to save it.
            Ogre::Vector3 topPos;

            NGF_SERIALISE_ON_SAVE
            {
                topPos = mTopNode->getPosition();
            }

            GRALL2_SERIALISE_GAMEOBJECT();

            NGF_SERIALISE_OGRE(Real, mFireTime);
            NGF_SERIALISE_OGRE(Real, mRestTime);
            NGF_SERIALISE_OGRE(Bool, mAlwaysScan);

            NGF_SERIALISE_OGRE(Int, mState);

            NGF_SERIALISE_OGRE(Real, mStateTimer);
            NGF_SERIALISE_OGRE(Real, mBulletTimer);

            NGF_SERIALISE_OGRE(Vector3, topPos);


            NGF_SERIALISE_ON_LOAD
            {
                mTopNode->setPosition(topPos);
            }
        }
        NGF_SERIALISE_END
};

#ifdef __TURRET_CPP__

/* C++ code produced by gperf version 3.0.4 *//*{{{*/
/* Command-line: gperf  */
/* Computed positions: -k'1,5' */

#if !((' ' == 32) && ('!' == 33) && ('"' == 34) && ('#' == 35) \
      && ('%' == 37) && ('&' == 38) && ('\'' == 39) && ('(' == 40) \
      && (')' == 41) && ('*' == 42) && ('+' == 43) && (',' == 44) \
      && ('-' == 45) && ('.' == 46) && ('/' == 47) && ('0' == 48) \
      && ('1' == 49) && ('2' == 50) && ('3' == 51) && ('4' == 52) \
      && ('5' == 53) && ('6' == 54) && ('7' == 55) && ('8' == 56) \
      && ('9' == 57) && (':' == 58) && (';' == 59) && ('<' == 60) \
      && ('=' == 61) && ('>' == 62) && ('?' == 63) && ('A' == 65) \
      && ('B' == 66) && ('C' == 67) && ('D' == 68) && ('E' == 69) \
      && ('F' == 70) && ('G' == 71) && ('H' == 72) && ('I' == 73) \
      && ('J' == 74) && ('K' == 75) && ('L' == 76) && ('M' == 77) \
      && ('N' == 78) && ('O' == 79) && ('P' == 80) && ('Q' == 81) \
      && ('R' == 82) && ('S' == 83) && ('T' == 84) && ('U' == 85) \
      && ('V' == 86) && ('W' == 87) && ('X' == 88) && ('Y' == 89) \
      && ('Z' == 90) && ('[' == 91) && ('\\' == 92) && (']' == 93) \
      && ('^' == 94) && ('_' == 95) && ('a' == 97) && ('b' == 98) \
      && ('c' == 99) && ('d' == 100) && ('e' == 101) && ('f' == 102) \
      && ('g' == 103) && ('h' == 104) && ('i' == 105) && ('j' == 106) \
      && ('k' == 107) && ('l' == 108) && ('m' == 109) && ('n' == 110) \
      && ('o' == 111) && ('p' == 112) && ('q' == 113) && ('r' == 114) \
      && ('s' == 115) && ('t' == 116) && ('u' == 117) && ('v' == 118) \
      && ('w' == 119) && ('x' == 120) && ('y' == 121) && ('z' == 122) \
      && ('{' == 123) && ('|' == 124) && ('}' == 125) && ('~' == 126))
/* The character set is not based on ISO-646.  */
#error "gperf generated tables don't work with this execution character set. Please report a bug to <bug-gnu-gperf@gnu.org>."
#endif


#ifndef __PYTHON_METHOD_STRUCT__
#define __PYTHON_METHOD_STRUCT__
struct PythonMethod
{
const char *name;
int code;
};
#endif //;
/* maximum key range = 18, duplicates = 0 */

class NGF_PY_CLASS_GPERF(Turret)
{
private:
  static inline unsigned int MakeHash (const char *str, unsigned int len);
public:
  static const struct PythonMethod *Lookup (const char *str, unsigned int len);
};

inline unsigned int
NGF_PY_CLASS_GPERF(Turret)::MakeHash (register const char *str, register unsigned int len)
{
  static const unsigned char asso_values[] =
    {
      28, 28, 28, 28, 28, 28, 28, 28, 28, 28,
      28, 28, 28, 28, 28, 28, 28, 28, 28, 28,
      28, 28, 28, 28, 28, 28, 28, 28, 28, 28,
      28, 28, 28, 28, 28, 28, 28, 28, 28, 28,
      28, 28, 28, 28, 28, 28, 28, 28, 28, 28,
      28, 28, 28, 28, 28, 28, 28, 28, 28, 28,
      28, 28, 28, 28, 28, 28, 28, 28, 28, 28,
      10, 28, 28, 28, 28, 28, 28, 28, 28, 28,
      28, 28, 28, 28, 28, 28, 28, 28, 28, 28,
      28, 28, 28, 28, 28, 28, 28,  0, 28, 28,
      28, 28, 10,  5, 28, 28, 28, 28, 28, 28,
      28, 28, 28, 28,  0,  0,  0, 28, 28, 28,
      28, 28, 28, 28, 28, 28, 28, 28, 28, 28,
      28, 28, 28, 28, 28, 28, 28, 28, 28, 28,
      28, 28, 28, 28, 28, 28, 28, 28, 28, 28,
      28, 28, 28, 28, 28, 28, 28, 28, 28, 28,
      28, 28, 28, 28, 28, 28, 28, 28, 28, 28,
      28, 28, 28, 28, 28, 28, 28, 28, 28, 28,
      28, 28, 28, 28, 28, 28, 28, 28, 28, 28,
      28, 28, 28, 28, 28, 28, 28, 28, 28, 28,
      28, 28, 28, 28, 28, 28, 28, 28, 28, 28,
      28, 28, 28, 28, 28, 28, 28, 28, 28, 28,
      28, 28, 28, 28, 28, 28, 28, 28, 28, 28,
      28, 28, 28, 28, 28, 28, 28, 28, 28, 28,
      28, 28, 28, 28, 28, 28, 28, 28, 28, 28,
      28, 28, 28, 28, 28, 28
    };
  return len + asso_values[(unsigned char)str[4]] + asso_values[(unsigned char)str[0]];
}

const struct PythonMethod *
NGF_PY_CLASS_GPERF(Turret)::Lookup (register const char *str, register unsigned int len)
{
  enum
    {
      TOTAL_KEYWORDS = 10,
      MIN_WORD_LENGTH = 10,
      MAX_WORD_LENGTH = 14,
      MIN_HASH_VALUE = 10,
      MAX_HASH_VALUE = 27
    };

  static const struct PythonMethod wordlist[] =
    {
      {"set_radius", NGF_PY_SET_GPERF(Turret, radius)},
      {"startFiring", NGF_PY_METHOD_GPERF(Turret, startFiring)},
      {"set_restTime", NGF_PY_SET_GPERF(Turret, restTime)},
      {"set_alwaysScan", NGF_PY_SET_GPERF(Turret, alwaysScan)},
      {"get_radius", NGF_PY_GET_GPERF(Turret, radius)},
      {"get_restTime", NGF_PY_GET_GPERF(Turret, restTime)},
      {"get_alwaysScan", NGF_PY_GET_GPERF(Turret, alwaysScan)},
      {"stopFiring", NGF_PY_METHOD_GPERF(Turret, stopFiring)},
      {"set_fireTime", NGF_PY_SET_GPERF(Turret, fireTime)},
      {"get_fireTime", NGF_PY_GET_GPERF(Turret, fireTime)}
    };

  if (len <= MAX_WORD_LENGTH && len >= MIN_WORD_LENGTH)
    {
      register int key = MakeHash (str, len);

      if (key <= MAX_HASH_VALUE && key >= MIN_HASH_VALUE)
        {
          register const struct PythonMethod *resword;

          switch (key - 10)
            {
              case 0:
                resword = &wordlist[0];
                goto compare;
              case 1:
                resword = &wordlist[1];
                goto compare;
              case 2:
                resword = &wordlist[2];
                goto compare;
              case 4:
                resword = &wordlist[3];
                goto compare;
              case 5:
                resword = &wordlist[4];
                goto compare;
              case 7:
                resword = &wordlist[5];
                goto compare;
              case 9:
                resword = &wordlist[6];
                goto compare;
              case 10:
                resword = &wordlist[7];
                goto compare;
              case 12:
                resword = &wordlist[8];
                goto compare;
              case 17:
                resword = &wordlist[9];
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
