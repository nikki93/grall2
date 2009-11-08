/*
 * =====================================================================================
 *
 *       Filename:  MovingBrush.h
 *
 *    Description:  Generic moving ('kinematic') brush. Highly scriptable.
 *
 *        Created:  05/29/2009 10:07:45 AM
 *       Compiler:  gcc
 *
 *         Author:  Nikhilesh (nikki)
 *
 * =====================================================================================
 */

#ifndef __MOVINGBRUSH_H__
#define __MOVINGBRUSH_H__

#include "Globals.h"
#include "Objects/Main/GraLL2GameObject.h"

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/vector.hpp>

#include <stack>

class MovingBrush :
    public GraLL2GameObject
{
    protected:
        btConvexHullShape *mShape;
        btConvexHullShape *mCastShape;
        Ogre::Entity *mEntity;

        Ogre::Vector3 mVelocity;
        bool mEnabled;
        bool mFollowDirectors;
        Ogre::Real mTimer;
        Ogre::Real mLastFrameTime;

        std::deque<Ogre::Vector3> mPoints;

    public:
        MovingBrush(Ogre::Vector3 pos, Ogre::Quaternion rot, NGF::ID id, NGF::PropertyList properties, Ogre::String name);
        virtual ~MovingBrush();

        //--- Events -------------------------------------------------------------------
        void postLoad();
        void unpausedTick(const Ogre::FrameEvent &evt);
        void pausedTick(const Ogre::FrameEvent &evt);
        NGF::MessageReply receiveMessage(NGF::Message msg);
        void collide(GameObject *other, btCollisionObject *otherPhysicsObject, btManifoldPoint &contact);

        //--- Python interface ---------------------------------------------------------
        NGF_PY_BEGIN_DECL(MovingBrush)
        {
            NGF_PY_METHOD_DECL(setPosition)
            NGF_PY_METHOD_DECL(setOrientation)
            NGF_PY_METHOD_DECL(addPoint)

            NGF_PY_PROPERTY_DECL(enabled)
            NGF_PY_PROPERTY_DECL(followDirectors)
            NGF_PY_PROPERTY_DECL(velocity)
        }
        NGF_PY_END_DECL

        //--- Serialisation ------------------------------------------------------------
        NGF_SERIALISE_BEGIN(MovingBrush)
        {
            std::vector<Ogre::String> pointsStrVec;

            NGF_SERIALISE_ON_SAVE
            {
                //Write each point.
                if (!mPoints.empty())
                    for (std::deque<Ogre::Vector3>::iterator iter = mPoints.begin(); iter != mPoints.end(); ++iter)
                        pointsStrVec.push_back(Ogre::StringConverter::toString(*iter));
            }

            GRALL2_SERIALISE_GAMEOBJECT();

            NGF_SERIALISE_OGRE(Vector3, mVelocity);
            NGF_SERIALISE_OGRE(Bool, mEnabled);
            NGF_SERIALISE_OGRE(Bool, mFollowDirectors);
            NGF_SERIALISE_OGRE(Real, mTimer);
            NGF_SERIALISE_OGRE(Real, mLastFrameTime);
            NGF_SERIALISE_STL_CONTAINER(pointsStrVec);

            NGF_SERIALISE_ON_LOAD
            {
                if (!pointsStrVec.empty())
                {
                    //Clear what we already have (actually supposed to be nothing, but we do this anyway).
                    mPoints.clear();

                    //Read it all back.
                    for (std::vector<Ogre::String>::iterator iter = pointsStrVec.begin(); iter != pointsStrVec.end(); ++iter)
                        mPoints.push_back(Ogre::StringConverter::parseVector3(*iter));
                }
            }
        }
        NGF_SERIALISE_END
};

#ifdef __MOVINGBRUSH_CPP__

/* C++ code produced by gperf version 3.0.3 *//*{{{*/
/* Command-line: gperf  */
/* Computed positions: -k'1,4' */

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
/* maximum key range = 22, duplicates = 0 */

class NGF_PY_CLASS_GPERF(MovingBrush)
{
private:
  static inline unsigned int MakeHash (const char *str, unsigned int len);
public:
  static const struct PythonMethod *Lookup (const char *str, unsigned int len);
};

inline unsigned int
NGF_PY_CLASS_GPERF(MovingBrush)::MakeHash (register const char *str, register unsigned int len)
{
  static const unsigned char asso_values[] =
    {
      30, 30, 30, 30, 30, 30, 30, 30, 30, 30,
      30, 30, 30, 30, 30, 30, 30, 30, 30, 30,
      30, 30, 30, 30, 30, 30, 30, 30, 30, 30,
      30, 30, 30, 30, 30, 30, 30, 30, 30, 30,
      30, 30, 30, 30, 30, 30, 30, 30, 30, 30,
      30, 30, 30, 30, 30, 30, 30, 30, 30, 30,
      30, 30, 30, 30, 30, 30, 30, 30, 30, 30,
      30, 30, 30, 30, 30, 30, 30, 30, 30,  0,
       0, 30, 30, 30, 30, 30, 30, 30, 30, 30,
      30, 30, 30, 30, 30,  5, 30,  0, 30, 30,
      30, 30, 30,  5, 30, 30, 30, 30, 30, 30,
      30, 30, 30, 30, 30,  0, 30, 30, 30, 30,
      30, 30, 30, 30, 30, 30, 30, 30, 30, 30,
      30, 30, 30, 30, 30, 30, 30, 30, 30, 30,
      30, 30, 30, 30, 30, 30, 30, 30, 30, 30,
      30, 30, 30, 30, 30, 30, 30, 30, 30, 30,
      30, 30, 30, 30, 30, 30, 30, 30, 30, 30,
      30, 30, 30, 30, 30, 30, 30, 30, 30, 30,
      30, 30, 30, 30, 30, 30, 30, 30, 30, 30,
      30, 30, 30, 30, 30, 30, 30, 30, 30, 30,
      30, 30, 30, 30, 30, 30, 30, 30, 30, 30,
      30, 30, 30, 30, 30, 30, 30, 30, 30, 30,
      30, 30, 30, 30, 30, 30, 30, 30, 30, 30,
      30, 30, 30, 30, 30, 30, 30, 30, 30, 30,
      30, 30, 30, 30, 30, 30, 30, 30, 30, 30,
      30, 30, 30, 30, 30, 30
    };
  return len + asso_values[(unsigned char)str[3]] + asso_values[(unsigned char)str[0]];
}

const struct PythonMethod *
NGF_PY_CLASS_GPERF(MovingBrush)::Lookup (register const char *str, register unsigned int len)
{
  enum
    {
      TOTAL_KEYWORDS = 9,
      MIN_WORD_LENGTH = 8,
      MAX_WORD_LENGTH = 19,
      MIN_HASH_VALUE = 8,
      MAX_HASH_VALUE = 29
    };

  static const struct PythonMethod wordlist[] =
    {
      {"addPoint", NGF_PY_METHOD_GPERF(MovingBrush, addPoint)},
      {"setPosition", NGF_PY_METHOD_GPERF(MovingBrush, setPosition)},
      {"setOrientation", NGF_PY_METHOD_GPERF(MovingBrush, setOrientation)},
      {"set_enabled", NGF_PY_SET_GPERF(MovingBrush, enabled)},
      {"set_velocity", NGF_PY_SET_GPERF(MovingBrush, velocity)},
      {"get_enabled", NGF_PY_GET_GPERF(MovingBrush, enabled)},
      {"get_velocity", NGF_PY_GET_GPERF(MovingBrush, velocity)},
      {"set_followDirectors", NGF_PY_SET_GPERF(MovingBrush, followDirectors)},
      {"get_followDirectors", NGF_PY_GET_GPERF(MovingBrush, followDirectors)}
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
              case 3:
                resword = &wordlist[1];
                goto compare;
              case 6:
                resword = &wordlist[2];
                goto compare;
              case 8:
                resword = &wordlist[3];
                goto compare;
              case 9:
                resword = &wordlist[4];
                goto compare;
              case 13:
                resword = &wordlist[5];
                goto compare;
              case 14:
                resword = &wordlist[6];
                goto compare;
              case 16:
                resword = &wordlist[7];
                goto compare;
              case 21:
                resword = &wordlist[8];
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
