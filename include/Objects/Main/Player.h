/*
 * =====================================================================================
 *
 *       Filename:  Player.h
 *
 *    Description:  The Player GameObject, which specifies the behaviour of the Player.
 *
 *        Created:  05/03/2009 03:04:54 PM
 *       Compiler:  gcc
 *
 *         Author:  Nikhilesh (nikki)
 *
 * =====================================================================================
 */

#ifndef __PLAYER_H__
#define __PLAYER_H__

#include "Globals.h"
#include "Objects/Main/GraLL2GameObject.h"

#include "BulletCollision/CollisionDispatch/btGhostObject.h"

class Player :
    public GraLL2GameObject
{
    protected:
        Ogre::SceneNode *mControlNode;
        Ogre::Entity *mEntity;
        btSphereShape *mShape;
        NGF::GameObject *mCameraHandler;

        bool mUnderControl; //It's all under control...
        bool mDead;

        Ogre::Real mMinHeight;

        NGF::GameObject *mLight;

        btSphereShape *mGhostShape;
        btPairCachingGhostObject *mGhostObject;

        //Our inventory. :-)
        typedef std::map<Ogre::String, unsigned int> PickupMap;
        PickupMap mPickups;

    public:
        Player(Ogre::Vector3 pos, Ogre::Quaternion rot, NGF::ID id, NGF::PropertyList properties, Ogre::String name);
        virtual ~Player();

        //--- Events -------------------------------------------------------------------
        void postLoad();
        void unpausedTick(const Ogre::FrameEvent &evt);
        void pausedTick(const Ogre::FrameEvent &evt);
        NGF::MessageReply receiveMessage(NGF::Message msg);
        void collide(GameObject *other, btCollisionObject *otherPhysicsObject, btManifoldPoint &contact);

        //--- Non-NGF ------------------------------------------------------------------
        void captureCameraHandler();
        void loseCameraHandler();
        void switchDimension();
        void die(bool explode);

        inline bool isKeyDown(OIS::KeyCode kc) { return (mUnderControl && Util::isKeyDown(kc)); }
        inline OIS::MouseState getMouseState() { return mUnderControl ? Util::getMouseState() : OIS::MouseState(); }

        //--- Python interface ---------------------------------------------------------
        NGF_PY_BEGIN_DECL(Player)
        {
            NGF_PY_METHOD_DECL(getControlOrientation)
            NGF_PY_METHOD_DECL(loseCameraHandler)
            NGF_PY_METHOD_DECL(captureCameraHandler)
            NGF_PY_METHOD_DECL(switchDimension)
            NGF_PY_METHOD_DECL(numPickup)
            NGF_PY_METHOD_DECL(hasPickup)
            NGF_PY_METHOD_DECL(incPickup)
            NGF_PY_METHOD_DECL(decPickup)
            NGF_PY_METHOD_DECL(hasDecPickup)

            NGF_PY_PROPERTY_DECL(underControl)
        }
        NGF_PY_END_DECL

        //--- Serialisation ------------------------------------------------------------
        NGF_SERIALISE_BEGIN(Player)
        {
            Ogre::Quaternion controlRot;

            NGF_SERIALISE_ON_SAVE
            {
                controlRot = mControlNode->getOrientation();
            }

            GRALL2_SERIALISE_GAMEOBJECT();

            NGF_SERIALISE_OGRE(Quaternion, controlRot);
            NGF_SERIALISE_OGRE(Bool, mUnderControl);
            NGF_SERIALISE_OGRE(Bool, mDead);
            NGF_SERIALISE_GAMEOBJECTPTR(mLight);

            NGF_SERIALISE_ON_LOAD
            {
                mControlNode->setOrientation(controlRot);
            }
        }
        NGF_SERIALISE_END
};

#ifdef __PLAYER_CPP__

/* C++ code produced by gperf version 3.0.4 *//*{{{*/
/* Command-line: gperf  */
/* Computed positions: -k'1' */

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

class NGF_PY_CLASS_GPERF(Player)
{
private:
  static inline unsigned int MakeHash (const char *str, unsigned int len);
public:
  static const struct PythonMethod *Lookup (const char *str, unsigned int len);
};

inline unsigned int
NGF_PY_CLASS_GPERF(Player)::MakeHash (register const char *str, register unsigned int len)
{
  static const unsigned char asso_values[] =
    {
      27, 27, 27, 27, 27, 27, 27, 27, 27, 27,
      27, 27, 27, 27, 27, 27, 27, 27, 27, 27,
      27, 27, 27, 27, 27, 27, 27, 27, 27, 27,
      27, 27, 27, 27, 27, 27, 27, 27, 27, 27,
      27, 27, 27, 27, 27, 27, 27, 27, 27, 27,
      27, 27, 27, 27, 27, 27, 27, 27, 27, 27,
      27, 27, 27, 27, 27, 27, 27, 27, 27, 27,
      27, 27, 27, 27, 27, 27, 27, 27, 27, 27,
      27, 27, 27, 27, 27, 27, 27, 27, 27, 27,
      27, 27, 27, 27, 27, 27, 27, 27, 27,  0,
      15, 27, 27,  5,  0, 10, 27, 27,  0, 27,
       5, 27, 27, 27, 27,  0, 27, 27, 27, 27,
      27, 27, 27, 27, 27, 27, 27, 27, 27, 27,
      27, 27, 27, 27, 27, 27, 27, 27, 27, 27,
      27, 27, 27, 27, 27, 27, 27, 27, 27, 27,
      27, 27, 27, 27, 27, 27, 27, 27, 27, 27,
      27, 27, 27, 27, 27, 27, 27, 27, 27, 27,
      27, 27, 27, 27, 27, 27, 27, 27, 27, 27,
      27, 27, 27, 27, 27, 27, 27, 27, 27, 27,
      27, 27, 27, 27, 27, 27, 27, 27, 27, 27,
      27, 27, 27, 27, 27, 27, 27, 27, 27, 27,
      27, 27, 27, 27, 27, 27, 27, 27, 27, 27,
      27, 27, 27, 27, 27, 27, 27, 27, 27, 27,
      27, 27, 27, 27, 27, 27, 27, 27, 27, 27,
      27, 27, 27, 27, 27, 27, 27, 27, 27, 27,
      27, 27, 27, 27, 27, 27
    };
  return len + asso_values[(unsigned char)str[0]];
}

const struct PythonMethod *
NGF_PY_CLASS_GPERF(Player)::Lookup (register const char *str, register unsigned int len)
{
  enum
    {
      TOTAL_KEYWORDS = 11,
      MIN_WORD_LENGTH = 9,
      MAX_WORD_LENGTH = 21,
      MIN_HASH_VALUE = 9,
      MAX_HASH_VALUE = 26
    };

  static const struct PythonMethod wordlist[] =
    {
      {"hasPickup", NGF_PY_METHOD_GPERF(Player, hasPickup)},
      {"hasDecPickup", NGF_PY_METHOD_GPERF(Player, hasDecPickup)},
      {"numPickup", NGF_PY_METHOD_GPERF(Player, numPickup)},
      {"switchDimension", NGF_PY_METHOD_GPERF(Player, switchDimension)},
      {"set_underControl", NGF_PY_SET_GPERF(Player, underControl)},
      {"loseCameraHandler", NGF_PY_METHOD_GPERF(Player, loseCameraHandler)},
      {"incPickup", NGF_PY_METHOD_GPERF(Player, incPickup)},
      {"captureCameraHandler", NGF_PY_METHOD_GPERF(Player, captureCameraHandler)},
      {"get_underControl", NGF_PY_GET_GPERF(Player, underControl)},
      {"decPickup", NGF_PY_METHOD_GPERF(Player, decPickup)},
      {"getControlOrientation", NGF_PY_METHOD_GPERF(Player, getControlOrientation)}
    };

  if (len <= MAX_WORD_LENGTH && len >= MIN_WORD_LENGTH)
    {
      register int key = MakeHash (str, len);

      if (key <= MAX_HASH_VALUE && key >= MIN_HASH_VALUE)
        {
          register const struct PythonMethod *resword;

          switch (key - 9)
            {
              case 0:
                resword = &wordlist[0];
                goto compare;
              case 3:
                resword = &wordlist[1];
                goto compare;
              case 5:
                resword = &wordlist[2];
                goto compare;
              case 6:
                resword = &wordlist[3];
                goto compare;
              case 7:
                resword = &wordlist[4];
                goto compare;
              case 8:
                resword = &wordlist[5];
                goto compare;
              case 10:
                resword = &wordlist[6];
                goto compare;
              case 11:
                resword = &wordlist[7];
                goto compare;
              case 12:
                resword = &wordlist[8];
                goto compare;
              case 15:
                resword = &wordlist[9];
                goto compare;
              case 17:
                resword = &wordlist[10];
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
