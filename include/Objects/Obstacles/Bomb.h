/*
 * =====================================================================================
 *
 *       Filename:  Bomb.h
 *
 *    Description:  A stationary Bomb. Don't touch! :P
 *
 *        Created:  06/14/2009 04:02:56 PM
 *       Compiler:  gcc
 *
 *         Author:  Nikhilesh (nikki)
 *
 * =====================================================================================
 */

#ifndef __BOMB_H__
#define __BOMB_H__

#include "Globals.h"
#include "Objects/Main/GraLL2GameObject.h"

class Bomb :
    public GraLL2GameObject
{
    protected:
        btCollisionShape *mShape;
        Ogre::Entity *mEntity;
        
        bool mGreen; //Green bombs are crate-fearing.
        bool mBulletSensitive;
        bool mExploded;

    public:
        Bomb(Ogre::Vector3 pos, Ogre::Quaternion rot, NGF::ID id, NGF::PropertyList properties, Ogre::String name);
        virtual ~Bomb();

        //--- Events -------------------------------------------------------------------
        void postLoad();
        void unpausedTick(const Ogre::FrameEvent &evt);
        void pausedTick(const Ogre::FrameEvent &evt);
        NGF::MessageReply receiveMessage(NGF::Message msg);
        void collide(GameObject *other, btCollisionObject *otherPhysicsObject, btManifoldPoint &contact);

        //--- Non-NGF ------------------------------------------------------------------
        void explode();

        //--- Python interface ---------------------------------------------------------
        NGF_PY_BEGIN_DECL(Bomb)
        {
            NGF_PY_METHOD_DECL(explode)
            NGF_PY_METHOD_DECL(setGreen)
        }
        NGF_PY_END_DECL

        //--- Serialisation ------------------------------------------------------------
        NGF_SERIALISE_BEGIN(Bomb)
        {
            GRALL2_SERIALISE_GAMEOBJECT();

            NGF_SERIALISE_OGRE(Bool, mExploded);
            NGF_SERIALISE_OGRE(Bool, mBulletSensitive);
            NGF_SERIALISE_OGRE(Bool, mGreen);
        }
        NGF_SERIALISE_END
};

#ifdef __BOMB_CPP__

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
/* maximum key range = 2, duplicates = 0 */

class NGF_PY_CLASS_GPERF(Bomb)
{
private:
  static inline unsigned int MakeHash (const char *str, unsigned int len);
public:
  static const struct PythonMethod *Lookup (const char *str, unsigned int len);
};

inline /*ARGSUSED*/
unsigned int
NGF_PY_CLASS_GPERF(Bomb)::MakeHash (register const char *str, register unsigned int len)
{
  return len;
}

const struct PythonMethod *
NGF_PY_CLASS_GPERF(Bomb)::Lookup (register const char *str, register unsigned int len)
{
  enum
    {
      TOTAL_KEYWORDS = 2,
      MIN_WORD_LENGTH = 7,
      MAX_WORD_LENGTH = 8,
      MIN_HASH_VALUE = 7,
      MAX_HASH_VALUE = 8
    };

  static const struct PythonMethod wordlist[] =
    {
      {"explode", NGF_PY_METHOD_GPERF(Bomb, explode)},
      {"setGreen", NGF_PY_METHOD_GPERF(Bomb, setGreen)}
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
              case 1:
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
