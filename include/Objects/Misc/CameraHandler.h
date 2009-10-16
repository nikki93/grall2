/*
 * =====================================================================================
 *
 *       Filename:  CameraHandler.h
 *
 *    Description:  A GameObject interface to Camera handling. Allows third-person
 *                  views, spline paths etc. Has an extreme hack for remembering the
 *                  target SceneNode.
 *
 *        Created:  05/12/2009 03:19:31 PM
 *       Compiler:  gcc
 *
 *         Author:  Nikhilesh (nikki)
 *
 * =====================================================================================
 */

#ifndef __CAMERAHANDLER_H__
#define __CAMERAHANDLER_H__

#include "Globals.h"

#include <sstream>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/map.hpp>

class CameraHandler :
    public NGF::Python::PythonGameObject,
    public NGF::Serialisation::SerialisableGameObject,
    public AlarmGameObject,
    public Util::ExtraEventListener
{
    public:
        enum
        {
            CS_NONE,
            CS_LOOKAT,
            CS_THIRDPERSON,
            CS_SPLINE,
            CS_DEATH, //Intended to go into this just after third person.
            CS_WIN,
        };

    protected:
        Ogre::Camera *mCamera;

        Ogre::SceneNode *mTargetNode;
        Ogre::String mTargetNodeName;

        Ogre::Vector3 mViewOffset;
        Ogre::Vector3 mLookAtOffset;
        Ogre::Degree mViewAngle;
        Ogre::Real mMovementFactor; //The fraction of the required displacement that the camera moves by (lerp).
        Ogre::Real mRotationFactor; //The fraction of the required rotation that the camera rotates by (lerp).

        Ogre::SceneNode *mSplineNode;
        Ogre::Animation *mSplineAnim;
        Ogre::AnimationState *mSplineAnimState;
        Ogre::NodeAnimationTrack *mSplineTrack;

        Ogre::Vector3 mGhostPos; //Position we were looking at when death happened.
        Ogre::Vector3 mGhostOffset;
        bool mGhostDirection;

        int mCurrState;

    public:
        CameraHandler(Ogre::Vector3 pos, Ogre::Quaternion rot, NGF::ID id, NGF::PropertyList properties, Ogre::String name);
        virtual ~CameraHandler();

        //--- Events -------------------------------------------------------------------
        void postLoad();
        void unpausedTick(const Ogre::FrameEvent &evt);
        void pausedTick(const Ogre::FrameEvent &evt);
        NGF::MessageReply receiveMessage(NGF::Message msg); 
        void alarm(Alarm a) { NGF_PY_CALL_EVENT(alarm, a); }

        //--- Non-NGF ------------------------------------------------------------------
        inline void lookAt(Ogre::Vector3 target, Ogre::Real elapsed)
        {
            Ogre::Vector3 dirA = mCamera->getDirection();
            Ogre::Vector3 dirB = target - mCamera->getPosition();
            dirB.normalise();
            Ogre::Vector3 res;

            if (mRotationFactor)
            {
                res = dirA + ((dirB - dirA) * elapsed * mRotationFactor);
            }
            else
            {
                res = dirB;
            }

            mCamera->setDirection(res);
        }

        //--- Python interface ---------------------------------------------------------
        NGF_PY_BEGIN_DECL(CameraHandler)
        {
            NGF_PY_METHOD_DECL(setAlarm)
            NGF_PY_METHOD_DECL(getPosition)
            NGF_PY_METHOD_DECL(setTarget)
            NGF_PY_METHOD_DECL(targetPlayer)
            //(totaltime)
            NGF_PY_METHOD_DECL(beginSpline)
            //(time, pointvec)
            NGF_PY_METHOD_DECL(addSplinePoint)
            NGF_PY_METHOD_DECL(endSpline)
            
            NGF_PY_PROPERTY_DECL(currState)
            NGF_PY_PROPERTY_DECL(movementFactor)
            NGF_PY_PROPERTY_DECL(rotationFactor)
            NGF_PY_PROPERTY_DECL(offset)
        }
        NGF_PY_END_DECL

        //--- Serialisation ------------------------------------------------------------
        NGF_SERIALISE_BEGIN(CameraHandler)
        {
            Ogre::String targetName;
            Ogre::Vector3 splineNodePos;

            Ogre::String splineStr;
            std::map<Ogre::Real, Ogre::String> splineStrMap;
            std::stringstream splineStream(std::stringstream::in | std::stringstream::out);
            Ogre::Real splineState;
            Ogre::Real splineLength;
            Ogre::Real viewAngle;

            //While saving, store stuff.
            NGF_SERIALISE_ON_SAVE
            {
                targetName = mTargetNode ? mTargetNode->getName() : "NULL";
                splineNodePos = mSplineNode->getPosition();

                //Save spline points (if any).
                if (mSplineAnim && mSplineAnimState && mSplineTrack)
                {
                    //Write to map.
                    unsigned short num = mSplineTrack->getNumKeyFrames();
                    for (unsigned short i = 0; i < num; ++i)
                    {
                        Ogre::TransformKeyFrame *key = static_cast<Ogre::TransformKeyFrame*>(mSplineTrack->getKeyFrame(i));
                        Ogre::Real time = key->getTime();
                        Ogre::String pos = Ogre::StringConverter::toString(key->getTranslate());
                        splineStrMap[time] = pos;
                    }

                    //Serialise map.
                    boost::archive::text_oarchive oa(splineStream);
                    oa << splineStrMap;
                    splineStr = splineStream.str();

                    //Save time.
                    splineState = mSplineAnimState->getTimePosition();
                    splineLength = mSplineAnim->getLength();
                }
                else
                {
                    splineStr = "n";
                }

                viewAngle = mViewAngle.valueDegrees();
            }

            //The actual read/write.
            NGF_SERIALISE_POSITION(mCamera->getPosition());
            NGF_SERIALISE_ROTATION(mCamera->getOrientation());
            NGF_SERIALISE_PYTHON_LOCALS();
            GRALL2_SERIALISE_ALARMS();

            NGF_SERIALISE_STRING(targetName);
            NGF_SERIALISE_OGRE(Int, mCurrState);
            NGF_SERIALISE_OGRE(Vector3, mViewOffset);
            NGF_SERIALISE_OGRE(Vector3, mLookAtOffset);
            NGF_SERIALISE_OGRE(Real, viewAngle);
            NGF_SERIALISE_OGRE(Real, mMovementFactor);
            NGF_SERIALISE_OGRE(Real, mRotationFactor);
            NGF_SERIALISE_STRING(splineStr);
            NGF_SERIALISE_OGRE(Real, splineState);
            NGF_SERIALISE_OGRE(Real, splineLength);
            NGF_SERIALISE_OGRE(Vector3, mGhostPos);
            NGF_SERIALISE_OGRE(Vector3, mGhostOffset);

            NGF_SERIALISE_ON_LOAD
            {
                mTargetNodeName = targetName;
                mSplineNode->setPosition(splineNodePos);

                if (splineStr != "n")
                {
                    //Clear/initialise stuff.
                    if (mSplineAnim)
                        GlbVar.ogreSmgr->destroyAnimation(mSplineAnim->getName());
                    if (mSplineAnimState)
                        GlbVar.ogreSmgr->destroyAnimationState(mSplineAnimState->getAnimationName());
                    if (mSplineTrack)
                        mSplineAnim->destroyNodeTrack(0);

                    mSplineAnim = GlbVar.ogreSmgr->createAnimation("cameraSpline", splineLength);
                    mSplineAnim->setInterpolationMode(Ogre::Animation::IM_SPLINE);

                    mSplineTrack = mSplineAnim->createNodeTrack(0, mSplineNode);

                    mSplineAnimState = GlbVar.ogreSmgr->createAnimationState("cameraSpline");
                    mSplineAnimState->setEnabled(true);
                    mSplineAnimState->setLoop(false);

                    //Deserialise map.
                    splineStream << splineStr;
                    boost::archive::text_iarchive ia(splineStream);
                    ia >> splineStrMap;

                    //Get keyframes from map.
                    for (std::map<Ogre::Real, Ogre::String>::iterator iter = splineStrMap.begin(); iter != splineStrMap.end(); ++iter)
                    {
                        Ogre::Real time = iter->first;
                        Ogre::Vector3 pos = Ogre::StringConverter::parseVector3(iter->second);

                        Ogre::TransformKeyFrame *key = mSplineTrack->createNodeKeyFrame(time);
                        key->setTranslate(pos);
                    }

                    //Jump back to saved time.
                    mSplineAnimState->setTimePosition(splineState);
                }

                mViewAngle = viewAngle;
            }
        }
        NGF_SERIALISE_END
};

#ifdef __CAMERAHANDLER_CPP__

/* C++ code produced by gperf version 3.0.3 *//*{{{*/
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
/* maximum key range = 26, duplicates = 0 */

class NGF_PY_CLASS_GPERF(CameraHandler)
{
private:
  static inline unsigned int MakeHash (const char *str, unsigned int len);
public:
  static const struct PythonMethod *Lookup (const char *str, unsigned int len);
};

inline unsigned int
NGF_PY_CLASS_GPERF(CameraHandler)::MakeHash (register const char *str, register unsigned int len)
{
  static const unsigned char asso_values[] =
    {
      34, 34, 34, 34, 34, 34, 34, 34, 34, 34,
      34, 34, 34, 34, 34, 34, 34, 34, 34, 34,
      34, 34, 34, 34, 34, 34, 34, 34, 34, 34,
      34, 34, 34, 34, 34, 34, 34, 34, 34, 34,
      34, 34, 34, 34, 34, 34, 34, 34, 34, 34,
      34, 34, 34, 34, 34, 34, 34, 34, 34, 34,
      34, 34, 34, 34, 34, 34, 34, 34, 34, 34,
      34, 34, 34, 34, 34, 34, 34, 34, 34, 34,
      34, 34, 34, 34, 34, 34, 34, 34, 34, 34,
      34, 34, 34, 34, 34, 34, 34,  0,  0,  4,
      34, 10, 34,  5, 34, 34, 34, 34,  0, 10,
       0,  0,  0, 34,  0,  0,  5, 34, 34, 34,
      34, 34, 34, 34, 34, 34, 34, 34, 34, 34,
      34, 34, 34, 34, 34, 34, 34, 34, 34, 34,
      34, 34, 34, 34, 34, 34, 34, 34, 34, 34,
      34, 34, 34, 34, 34, 34, 34, 34, 34, 34,
      34, 34, 34, 34, 34, 34, 34, 34, 34, 34,
      34, 34, 34, 34, 34, 34, 34, 34, 34, 34,
      34, 34, 34, 34, 34, 34, 34, 34, 34, 34,
      34, 34, 34, 34, 34, 34, 34, 34, 34, 34,
      34, 34, 34, 34, 34, 34, 34, 34, 34, 34,
      34, 34, 34, 34, 34, 34, 34, 34, 34, 34,
      34, 34, 34, 34, 34, 34, 34, 34, 34, 34,
      34, 34, 34, 34, 34, 34, 34, 34, 34, 34,
      34, 34, 34, 34, 34, 34, 34, 34, 34, 34,
      34, 34, 34, 34, 34, 34
    };
  return len + asso_values[(unsigned char)str[4]] + asso_values[(unsigned char)str[0]];
}

const struct PythonMethod *
NGF_PY_CLASS_GPERF(CameraHandler)::Lookup (register const char *str, register unsigned int len)
{
  enum
    {
      TOTAL_KEYWORDS = 15,
      MIN_WORD_LENGTH = 8,
      MAX_WORD_LENGTH = 18,
      MIN_HASH_VALUE = 8,
      MAX_HASH_VALUE = 33
    };

  static const struct PythonMethod wordlist[] =
    {
      {"setAlarm", NGF_PY_METHOD_GPERF(CameraHandler, setAlarm)},
      {"setTarget", NGF_PY_METHOD_GPERF(CameraHandler, setTarget)},
      {"set_offset", NGF_PY_SET_GPERF(CameraHandler, offset)},
      {"beginSpline", NGF_PY_METHOD_GPERF(CameraHandler, beginSpline)},
      {"addSplinePoint", NGF_PY_METHOD_GPERF(CameraHandler, addSplinePoint)},
      {"get_offset", NGF_PY_GET_GPERF(CameraHandler, offset)},
      {"getPosition", NGF_PY_METHOD_GPERF(CameraHandler, getPosition)},
      {"set_currState", NGF_PY_SET_GPERF(CameraHandler, currState)},
      {"set_rotationFactor", NGF_PY_SET_GPERF(CameraHandler, rotationFactor)},
      {"endSpline", NGF_PY_METHOD_GPERF(CameraHandler, endSpline)},
      {"get_currState", NGF_PY_GET_GPERF(CameraHandler, currState)},
      {"get_rotationFactor", NGF_PY_GET_GPERF(CameraHandler, rotationFactor)},
      {"targetPlayer", NGF_PY_METHOD_GPERF(CameraHandler, targetPlayer)},
      {"set_movementFactor", NGF_PY_SET_GPERF(CameraHandler, movementFactor)},
      {"get_movementFactor", NGF_PY_GET_GPERF(CameraHandler, movementFactor)}
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
              case 1:
                resword = &wordlist[1];
                goto compare;
              case 2:
                resword = &wordlist[2];
                goto compare;
              case 3:
                resword = &wordlist[3];
                goto compare;
              case 6:
                resword = &wordlist[4];
                goto compare;
              case 7:
                resword = &wordlist[5];
                goto compare;
              case 8:
                resword = &wordlist[6];
                goto compare;
              case 9:
                resword = &wordlist[7];
                goto compare;
              case 10:
                resword = &wordlist[8];
                goto compare;
              case 11:
                resword = &wordlist[9];
                goto compare;
              case 14:
                resword = &wordlist[10];
                goto compare;
              case 15:
                resword = &wordlist[11];
                goto compare;
              case 19:
                resword = &wordlist[12];
                goto compare;
              case 20:
                resword = &wordlist[13];
                goto compare;
              case 25:
                resword = &wordlist[14];
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
