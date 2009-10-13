/*
 * =====================================================================================
 *
 *       Filename:  MovingBomb.h
 *
 *    Description:  A bomb. Only, it moves.
 *
 *        Created:  07/06/2009 01:07:45 PM
 *       Compiler:  gcc
 *
 *         Author:  Nikhilesh (nikki)
 *
 * =====================================================================================
 */

#ifndef __MOVINGBOMB_H__
#define __MOVINGBOMB_H__

#include "Globals.h"
#include "Objects/Main/GraLL2GameObject.h"

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/vector.hpp>

#include <stack>

class MovingBomb :
    public GraLL2GameObject
{
    protected:
        btSphereShape *mShape;
        btSphereShape *mCastShape;
        Ogre::Entity *mEntity;

        Ogre::Vector3 mVelocity;
        bool mEnabled;
        bool mExploded;
        bool mFollowDirectors;
        bool mBulletSensitive;
        Ogre::Real mTimer;
        Ogre::Real mLastFrameTime;

        std::deque<Ogre::Vector3> mPoints;

    public:
        MovingBomb(Ogre::Vector3 pos, Ogre::Quaternion rot, NGF::ID id, NGF::PropertyList properties, Ogre::String name);
        virtual ~MovingBomb();

        //--- Events -------------------------------------------------------------------
        void postLoad();
        void unpausedTick(const Ogre::FrameEvent &evt);
        void pausedTick(const Ogre::FrameEvent &evt);
        NGF::MessageReply receiveMessage(NGF::Message msg);
        void collide(GameObject *other, btCollisionObject *otherPhysicsObject, btManifoldPoint &contact);

        //--- Non-NGF ------------------------------------------------------------------
        void explode();

        //--- Python interface ---------------------------------------------------------
        NGF_PY_BEGIN_DECL(MovingBomb)
        {
            NGF_PY_METHOD_DECL(setPosition)
            NGF_PY_METHOD_DECL(setOrientation)
            NGF_PY_METHOD_DECL(addPoint)
            NGF_PY_METHOD_DECL(explode)

            NGF_PY_PROPERTY_DECL(enabled)
            NGF_PY_PROPERTY_DECL(followDirectors)
            NGF_PY_PROPERTY_DECL(velocity)
        }
        NGF_PY_END_DECL

        //--- Serialisation ------------------------------------------------------------
        NGF_SERIALISE_BEGIN(MovingBomb)
        {
            std::vector<Ogre::String> pointsStrVec;
            Ogre::String pointsStr;
            std::stringstream pointsStream(std::stringstream::in | std::stringstream::out);

            NGF_SERIALISE_ON_SAVE
            {
                if (!mPoints.empty())
                {
                    //Write each point.
                    for (std::deque<Ogre::Vector3>::iterator iter = mPoints.begin(); iter != mPoints.end(); ++iter)
                        pointsStrVec.push_back(Ogre::StringConverter::toString(*iter));

                    //Serialise all.
                    boost::archive::text_oarchive oa(pointsStream);
                    oa << pointsStrVec;
                    pointsStr = pointsStream.str();
                }
                else
                {
                    pointsStr = "n";
                }
            }

            GRALL2_SERIALISE_GAMEOBJECT();

            NGF_SERIALISE_OGRE(Vector3, mVelocity);
            NGF_SERIALISE_OGRE(Bool, mEnabled);
            NGF_SERIALISE_OGRE(Bool, mFollowDirectors);
            NGF_SERIALISE_OGRE(Bool, mBulletSensitive);
            NGF_SERIALISE_OGRE(Real, mTimer);
            NGF_SERIALISE_OGRE(Real, mLastFrameTime);
            NGF_SERIALISE_OGRE(Bool, mExploded);

            NGF_SERIALISE_STRING(pointsStr);

            NGF_SERIALISE_ON_LOAD
            {
                if (pointsStr != "n")
                {
                    //Clear what we already have (actually supposed to be nothing, but we do this anyway).
                    mPoints.clear();

                    //Deserialise.
                    pointsStream << pointsStr;
                    boost::archive::text_iarchive ia(pointsStream);
                    ia >> pointsStrVec;

                    //Read it all back.
                    for (std::vector<Ogre::String>::iterator iter = pointsStrVec.begin(); iter != pointsStrVec.end(); ++iter)
                        mPoints.push_back(Ogre::StringConverter::parseVector3(*iter));
                }
            }
        }
        NGF_SERIALISE_END
};

#ifdef __MOVINGBOMB_CPP__

/* C++ code produced by gperf version 3.0.4 *//*{{{*/
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
/* maximum key range = 18, duplicates = 0 */

class NGF_PY_CLASS_GPERF(MovingBomb)
{
private:
  static inline unsigned int MakeHash (const char *str, unsigned int len);
public:
  static const struct PythonMethod *Lookup (const char *str, unsigned int len);
};

inline unsigned int
NGF_PY_CLASS_GPERF(MovingBomb)::MakeHash (register const char *str, register unsigned int len)
{
  static const unsigned char asso_values[] =
    {
      25, 25, 25, 25, 25, 25, 25, 25, 25, 25,
      25, 25, 25, 25, 25, 25, 25, 25, 25, 25,
      25, 25, 25, 25, 25, 25, 25, 25, 25, 25,
      25, 25, 25, 25, 25, 25, 25, 25, 25, 25,
      25, 25, 25, 25, 25, 25, 25, 25, 25, 25,
      25, 25, 25, 25, 25, 25, 25, 25, 25, 25,
      25, 25, 25, 25, 25, 25, 25, 25, 25, 25,
      25, 25, 25, 25, 25, 25, 25, 25, 25,  0,
      10, 25, 25, 25, 25, 25, 25, 25, 25, 25,
      25, 25, 25, 25, 25,  0, 25,  0, 25, 25,
      25,  0, 25,  5, 25, 25, 25, 25,  0, 25,
      25, 25, 25, 25, 25,  0, 25, 25, 25, 25,
      25, 25, 25, 25, 25, 25, 25, 25, 25, 25,
      25, 25, 25, 25, 25, 25, 25, 25, 25, 25,
      25, 25, 25, 25, 25, 25, 25, 25, 25, 25,
      25, 25, 25, 25, 25, 25, 25, 25, 25, 25,
      25, 25, 25, 25, 25, 25, 25, 25, 25, 25,
      25, 25, 25, 25, 25, 25, 25, 25, 25, 25,
      25, 25, 25, 25, 25, 25, 25, 25, 25, 25,
      25, 25, 25, 25, 25, 25, 25, 25, 25, 25,
      25, 25, 25, 25, 25, 25, 25, 25, 25, 25,
      25, 25, 25, 25, 25, 25, 25, 25, 25, 25,
      25, 25, 25, 25, 25, 25, 25, 25, 25, 25,
      25, 25, 25, 25, 25, 25, 25, 25, 25, 25,
      25, 25, 25, 25, 25, 25, 25, 25, 25, 25,
      25, 25, 25, 25, 25, 25
    };
  return len + asso_values[(unsigned char)str[3]] + asso_values[(unsigned char)str[0]];
}

const struct PythonMethod *
NGF_PY_CLASS_GPERF(MovingBomb)::Lookup (register const char *str, register unsigned int len)
{
  enum
    {
      TOTAL_KEYWORDS = 10,
      MIN_WORD_LENGTH = 7,
      MAX_WORD_LENGTH = 19,
      MIN_HASH_VALUE = 7,
      MAX_HASH_VALUE = 24
    };

  static const struct PythonMethod wordlist[] =
    {
      {"explode", NGF_PY_METHOD_GPERF(MovingBomb, explode)},
      {"set_enabled", NGF_PY_SET_GPERF(MovingBomb, enabled)},
      {"set_velocity", NGF_PY_SET_GPERF(MovingBomb, velocity)},
      {"setOrientation", NGF_PY_METHOD_GPERF(MovingBomb, setOrientation)},
      {"get_enabled", NGF_PY_GET_GPERF(MovingBomb, enabled)},
      {"get_velocity", NGF_PY_GET_GPERF(MovingBomb, velocity)},
      {"addPoint", NGF_PY_METHOD_GPERF(MovingBomb, addPoint)},
      {"set_followDirectors", NGF_PY_SET_GPERF(MovingBomb, followDirectors)},
      {"setPosition", NGF_PY_METHOD_GPERF(MovingBomb, setPosition)},
      {"get_followDirectors", NGF_PY_GET_GPERF(MovingBomb, followDirectors)}
    };

  if (len <= MAX_WORD_LENGTH && len >= MIN_WORD_LENGTH)
    {
      register int key = MakeHash (str, len);

      if (key <= MAX_HASH_VALUE && key >= MIN_HASH_VALUE)
        {
          register const struct PythonMethod *resword;

          switch (key - 7)
            {
              case 0:
                resword = &wordlist[0];
                goto compare;
              case 4:
                resword = &wordlist[1];
                goto compare;
              case 5:
                resword = &wordlist[2];
                goto compare;
              case 7:
                resword = &wordlist[3];
                goto compare;
              case 9:
                resword = &wordlist[4];
                goto compare;
              case 10:
                resword = &wordlist[5];
                goto compare;
              case 11:
                resword = &wordlist[6];
                goto compare;
              case 12:
                resword = &wordlist[7];
                goto compare;
              case 14:
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
