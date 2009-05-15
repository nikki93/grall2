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

class Player :
    public GraLL2GameObject
{
    protected:
        Ogre::SceneNode *mControlNode;
        Ogre::Entity *mEntity;
        btCollisionShape *mShape;
        NGF::GameObject *mCameraHandler;

        bool mUnderControl; //Are we under control?

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
        inline bool isKeyDown(OIS::KeyCode kc) { return (mUnderControl && ::isKeyDown(kc)); }
        inline OIS::MouseState getMouseState() { if (mUnderControl) return ::getMouseState(); else return OIS::MouseState(); }

        //--- Python interface ---------------------------------------------------------
        NGF_PY_BEGIN_DECL(Player)
        {
            NGF_PY_METHOD_DECL(getControlOrientation)
            NGF_PY_METHOD_DECL(setControl)
            NGF_PY_METHOD_DECL(loseCameraHandler)
            NGF_PY_METHOD_DECL(captureCameraHandler)
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

            NGF_SERIALISE_ON_LOAD
            {
                mControlNode->setOrientation(controlRot);
            }
        }
        NGF_SERIALISE_END
};

#ifdef __PLAYER_CPP__

/* C++ code produced by gperf version 3.0.3 *//*{{{*/
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
/* maximum key range = 12, duplicates = 0 */

class NGF_PY_CLASS_GPERF(Player)
{
private:
  static inline unsigned int MakeHash (const char *str, unsigned int len);
public:
  static const struct PythonMethod *Lookup (const char *str, unsigned int len);
};

inline /*ARGSUSED*/
unsigned int
NGF_PY_CLASS_GPERF(Player)::MakeHash (register const char *str, register unsigned int len)
{
  return len;
}

const struct PythonMethod *
NGF_PY_CLASS_GPERF(Player)::Lookup (register const char *str, register unsigned int len)
{
  enum
    {
      TOTAL_KEYWORDS = 4,
      MIN_WORD_LENGTH = 10,
      MAX_WORD_LENGTH = 21,
      MIN_HASH_VALUE = 10,
      MAX_HASH_VALUE = 21
    };

  static const struct PythonMethod wordlist[] =
    {
      {"setControl", NGF_PY_METHOD_GPERF(Player, setControl)},
      {"loseCameraHandler", NGF_PY_METHOD_GPERF(Player, loseCameraHandler)},
      {"captureCameraHandler", NGF_PY_METHOD_GPERF(Player, captureCameraHandler)},
      {"getControlOrientation", NGF_PY_METHOD_GPERF(Player, getControlOrientation)}
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
              case 7:
                resword = &wordlist[1];
                goto compare;
              case 10:
                resword = &wordlist[2];
                goto compare;
              case 11:
                resword = &wordlist[3];
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
