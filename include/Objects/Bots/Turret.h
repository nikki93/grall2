/*
 * =====================================================================================
 *
 *       Filename:  Turret.h
 *
 *    Description:  The Turret GameObject. Annoys the Player for a while and then goes
 *                  into hibernation. Repeats. :P
 *
 *        Created:  09/18/2009 12:31:23 AM
 *       Compiler:  gcc
 *
 *         Author:  Nikhilesh (nikki)
 *
 * =====================================================================================
 */

#ifndef __TURRET_H__
#define __TURRET_H__

#include "Globals.h"
#include "Objects/Main/GraLL2GameObject.h"

#include <boost/serialization/vector.hpp>
#include <boost/preprocessor.hpp>

#define TURRET_TOP_HEIGHT 0.16f
#define TURRET_FIRST_BULLET_TIME 1

//Move Turret head up or down. Just pushes a temporary move state on stack, which pops
//itself off when done. If already in position, skip.
#define TURRET_HEAD_UP()                                                               \
    if (mObj->mTopNode->getPosition().y < 0)                                           \
        NGF_STATES_CONTAINER_PUSH_STATE(MoveUp)
#define TURRET_HEAD_DOWN()                                                             \
    if (mObj->mTopNode->getPosition().y > -TURRET_TOP_HEIGHT)                          \
        NGF_STATES_CONTAINER_PUSH_STATE(MoveDown)

class Turret :
    public GraLL2GameObject
{
    protected:
        btCollisionShape *mShape;

        Ogre::SceneNode *mTopNode;
        Ogre::SceneNode *mBaseNode;
        Ogre::Entity *mTopEntity;
        Ogre::Entity *mBaseEntity;

        OgreAL::Sound *mShootSound;
        OgreAL::Sound *mMoveSound;

        Ogre::Real mRadius;
        bool mAlwaysScan;
        Ogre::Real mFireTime;
        Ogre::Real mRestTime;

        //--- States -------------------------------------------------------------------
        
        Ogre::Real mTime;
        Ogre::Real mBulletTime;

        void fire(Ogre::Real time);
        void rest(Ogre::Real time);
        void scan();
        void disable();
        void enable();

        NGF_STATES_INTERFACE_BEGIN(Turret)
        {
            virtual void unpausedTick(const Ogre::FrameEvent &evt) {}
        }
        NGF_STATES_INTERFACE_END

        //Firing.
        NGF_STATES_STATE_BEGIN(Fire)
        {
            void enter() { TURRET_HEAD_UP(); mObj->mBulletTime = TURRET_FIRST_BULLET_TIME; }
            void unpausedTick(const Ogre::FrameEvent &evt);
        }
        NGF_STATES_STATE_END(Fire)

        //Disabled (waiting for next enable).
        NGF_STATES_STATE_BEGIN(Disabled)
        {
            void enter() { TURRET_HEAD_DOWN(); }
            //unpausedTick is default (do nothing).
        }
        NGF_STATES_STATE_END(Disabled)

        //Rest (waiting for next fire time).
        NGF_STATES_STATE_BEGIN(Rest)
        {
            void enter() { TURRET_HEAD_DOWN(); }
            void unpausedTick(const Ogre::FrameEvent &evt);
        }
        NGF_STATES_STATE_END(Rest)

        //Scanning (when see Player, start firing).
        NGF_STATES_STATE_BEGIN(Scan)
        {
            void enter() { TURRET_HEAD_DOWN(); }
            void unpausedTick(const Ogre::FrameEvent &evt);
        }
        NGF_STATES_STATE_END(Scan)

        //Moving head down.
        NGF_STATES_STATE_BEGIN(MoveDown)
        {
            //Moving sound.
            void enter() { mObj->mMoveSound->play(); mObj->mMoveSound->setSecondOffset(0.54); }
            //void exit() { mObj->mMoveSound->stop(); }
            void unpausedTick(const Ogre::FrameEvent &evt);
        }
        NGF_STATES_STATE_END(MoveDown)

        //Moving head up.
        NGF_STATES_STATE_BEGIN(MoveUp)
        {
            //Moving sound.
            void enter() { mObj->mMoveSound->play(); }
            //void exit() { mObj->mMoveSound->stop(); }
            void unpausedTick(const Ogre::FrameEvent &evt);
        }
        NGF_STATES_STATE_END(MoveUp)

    public:
        Turret(Ogre::Vector3 pos, Ogre::Quaternion rot, NGF::ID id, NGF::PropertyList properties, Ogre::String name);
        virtual ~Turret();

        //--- Events -------------------------------------------------------------------
        void postLoad();
        void unpausedTick(const Ogre::FrameEvent &evt);
        void pausedTick(const Ogre::FrameEvent &evt);
        NGF::MessageReply receiveMessage(NGF::Message msg);
        void collide(GameObject *other, btCollisionObject *otherPhysicsObject, btManifoldPoint &contact);

        //--- Some helper functions ----------------------------------------------------
        void fireSingleBullet();
        bool doSingleScan();
        bool playerIsInRadius();

        //--- Python interface ---------------------------------------------------------
        NGF_PY_BEGIN_DECL(Turret)
        {
            NGF_PY_METHOD_DECL(fire)
            NGF_PY_METHOD_DECL(stop)
            NGF_PY_METHOD_DECL(enable)
            NGF_PY_METHOD_DECL(disable)

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

            std::vector<unsigned int> stateInds;                                               
            int currState, nextState;                                                                    
                                                                                               
            NGF_SERIALISE_ON_SAVE                                                              
            {                                                                                  
                for (std::vector<__State *>::iterator iter = __mStateStack.begin();            
                        iter != __mStateStack.end(); ++iter)                                   
                    stateInds.push_back((*iter)->getIndex());                                 
                                                                                               
                currState = mCurrState ? mCurrState->getIndex() : 0;                           
                nextState = __mNextState ? __mNextState->getIndex() : 0;                           
            }                                                                                  
                                                                                               
            NGF_SERIALISE_STL_CONTAINER(stateInds);                                            
            NGF_SERIALISE_OGRE(Int, currState);                                                
                                                                                               
            NGF_SERIALISE_ON_LOAD                                                              
            {                                                                                  
                __mStateStack.clear();                                                         
                for (std::vector<unsigned int>::iterator iter = stateInds.begin();             
                        iter != stateInds.end(); ++iter)                                       
                    __mStateStack.push_back(NGF_STATES_GET_STATE_FROM_INDEX(*iter));           
                                                                                               
                mCurrState = currState ? NGF_STATES_GET_STATE_FROM_INDEX(currState) : 0;       
                __mNextState = nextState ? NGF_STATES_GET_STATE_FROM_INDEX(nextState) : 0;       
            }

            NGF_SERIALISE_OGRE(Real, mRadius);
            NGF_SERIALISE_OGRE(Bool, mAlwaysScan);
            NGF_SERIALISE_OGRE(Real, mFireTime);
            NGF_SERIALISE_OGRE(Real, mRestTime);

            NGF_SERIALISE_OGRE(Real, mTime);
            NGF_SERIALISE_OGRE(Real, mBulletTime);

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
/* maximum key range = 24, duplicates = 0 */

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
      28, 28, 28, 28, 28, 28, 28, 28, 28, 28,
      28, 28, 28, 28, 28, 28, 28, 28, 28, 28,
      28, 28, 28, 28, 28, 28, 28,  5,  0, 28,
       0,  0, 10,  5, 28, 28, 28, 28,  0, 28,
      28, 28, 28, 28,  0,  0, 28, 28, 28, 28,
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
  register int hval = len;

  switch (hval)
    {
      default:
        hval += asso_values[(unsigned char)str[4]];
      /*FALLTHROUGH*/
      case 4:
      case 3:
      case 2:
      case 1:
        hval += asso_values[(unsigned char)str[0]];
        break;
    }
  return hval;
}

const struct PythonMethod *
NGF_PY_CLASS_GPERF(Turret)::Lookup (register const char *str, register unsigned int len)
{
  enum
    {
      TOTAL_KEYWORDS = 12,
      MIN_WORD_LENGTH = 4,
      MAX_WORD_LENGTH = 14,
      MIN_HASH_VALUE = 4,
      MAX_HASH_VALUE = 27
    };

  static const struct PythonMethod wordlist[] =
    {
      {""}, {""}, {""}, {""},
      {"stop", NGF_PY_METHOD_GPERF(Turret, stop)},
      {""},
      {"enable", NGF_PY_METHOD_GPERF(Turret, enable)},
      {"disable", NGF_PY_METHOD_GPERF(Turret, disable)},
      {""}, {""},
      {"set_radius", NGF_PY_SET_GPERF(Turret, radius)},
      {""},
      {"set_restTime", NGF_PY_SET_GPERF(Turret, restTime)},
      {""},
      {"fire", NGF_PY_METHOD_GPERF(Turret, fire)},
      {"get_radius", NGF_PY_GET_GPERF(Turret, radius)},
      {""},
      {"get_restTime", NGF_PY_GET_GPERF(Turret, restTime)},
      {""},
      {"set_alwaysScan", NGF_PY_SET_GPERF(Turret, alwaysScan)},
      {""}, {""},
      {"set_fireTime", NGF_PY_SET_GPERF(Turret, fireTime)},
      {""},
      {"get_alwaysScan", NGF_PY_GET_GPERF(Turret, alwaysScan)},
      {""}, {""},
      {"get_fireTime", NGF_PY_GET_GPERF(Turret, fireTime)}
    };

  if (len <= MAX_WORD_LENGTH && len >= MIN_WORD_LENGTH)
    {
      register int key = MakeHash (str, len);

      if (key <= MAX_HASH_VALUE && key >= 0)
        {
          register const char *s = wordlist[key].name;

          if (*str == *s && !strcmp (str + 1, s + 1))
            return &wordlist[key];
        }
    }
  return 0;
}/*}}}*/

#endif

#endif
