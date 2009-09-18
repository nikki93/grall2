/*
 * =====================================================================================
 *
 *       Filename:  Bullet.h
 *
 *    Description:  A Bullet. Incoming! :-)
 *
 *        Created:  09/17/2009 05:49:54 PM
 *       Compiler:  gcc
 *
 *         Author:  Nikhilesh (nikki)
 *
 * =====================================================================================
 */

#ifndef __BULLET_H__
#define __BULLET_H__

#include "Globals.h"
#include "Objects/Main/GraLL2GameObject.h"

class Bullet :
    public GraLL2GameObject
{
    protected:
        btCylinderShape *mShape;
        Ogre::Entity *mEntity;

        bool mExploded;
        Ogre::Real mSpeed;

        Ogre::Real mLifetime;

    public:
        Bullet(Ogre::Vector3 pos, Ogre::Quaternion rot, NGF::ID id, NGF::PropertyList properties, Ogre::String name);
        virtual ~Bullet();

        //--- Events -------------------------------------------------------------------
        void postLoad();
        void unpausedTick(const Ogre::FrameEvent &evt);
        void pausedTick(const Ogre::FrameEvent &evt);
        NGF::MessageReply receiveMessage(NGF::Message msg);
        void collide(GameObject *other, btCollisionObject *otherPhysicsObject, btManifoldPoint &contact);

        //--- Non-NGF ------------------------------------------------------------------
        void explode();

        //--- Python interface ---------------------------------------------------------
        NGF_PY_BEGIN_DECL(Bullet)
        {
            NGF_PY_METHOD_DECL(explode)
        }
        NGF_PY_END_DECL

        //--- Serialisation ------------------------------------------------------------
        NGF_SERIALISE_BEGIN(Bullet)
        {
            GRALL2_SERIALISE_GAMEOBJECT();

            NGF_SERIALISE_OGRE(Bool, mExploded);
            NGF_SERIALISE_OGRE(Real, mSpeed);
            NGF_SERIALISE_OGRE(Real, mLifetime);
        }
        NGF_SERIALISE_END
};

#ifdef __BULLET_CPP__

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
/* maximum key range = 1, duplicates = 0 */

class NGF_PY_CLASS_GPERF(Bullet)
{
private:
  static inline unsigned int MakeHash (const char *str, unsigned int len);
public:
  static const struct PythonMethod *Lookup (const char *str, unsigned int len);
};

inline /*ARGSUSED*/
unsigned int
NGF_PY_CLASS_GPERF(Bullet)::MakeHash (register const char *str, register unsigned int len)
{
  return 0;
}

const struct PythonMethod *
NGF_PY_CLASS_GPERF(Bullet)::Lookup (register const char *str, register unsigned int len)
{
  enum
    {
      TOTAL_KEYWORDS = 1,
      MIN_WORD_LENGTH = 7,
      MAX_WORD_LENGTH = 7,
      MIN_HASH_VALUE = 0,
      MAX_HASH_VALUE = 0
    };

  static const struct PythonMethod wordlist[] =
    {
      {"explode", NGF_PY_METHOD_GPERF(Bullet, explode)}
    };

  if (len <= MAX_WORD_LENGTH && len >= MIN_WORD_LENGTH)
    {
      register int key = MakeHash (str, len);

      if (key <= MAX_HASH_VALUE && key >= MIN_HASH_VALUE)
        {
          register const struct PythonMethod *resword;

          resword = &wordlist[0];
          goto compare;
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
