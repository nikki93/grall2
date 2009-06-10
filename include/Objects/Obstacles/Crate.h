/*
 * =====================================================================================
 *
 *       Filename:  Crate.h
 *
 *    Description:  A block you can push around.
 *
 *        Created:  06/05/2009 08:29:49 PM
 *       Compiler:  gcc
 *
 *         Author:  Nikhilesh (nikki)
 *
 * =====================================================================================
 */

#ifndef __CRATE_H__
#define __CRATE_H__

#include "Globals.h"
#include "Objects/Main/GraLL2GameObject.h"

class Crate :
    public GraLL2GameObject
{
    protected:
        Ogre::Entity *mEntity;

        btBoxShape *mShape;
        btBoxShape *mShape2;
        btRigidBody *mFixedBody;
        btSliderConstraint *mConstraint;

        bool mMoving;
        Ogre::Vector3 mNextDirection;
        Ogre::Real mDistanceMoved;
        Ogre::Real mSize;

    public:
        Crate(Ogre::Vector3 pos, Ogre::Quaternion rot, NGF::ID id, NGF::PropertyList properties, Ogre::String name);
        virtual ~Crate();

        //--- Events -------------------------------------------------------------------
        void postLoad();
        void unpausedTick(const Ogre::FrameEvent &evt);
        void pausedTick(const Ogre::FrameEvent &evt);
        NGF::MessageReply receiveMessage(NGF::Message msg);
        void collide(GameObject *other, btCollisionObject *otherPhysicsObject, btManifoldPoint &contact);

        //--- Non-NGF ------------------------------------------------------------------
        void makeMove(const Ogre::Vector3 &dir);

        //--- Python interface ---------------------------------------------------------
        NGF_PY_BEGIN_DECL(Crate)
        {
            NGF_PY_METHOD_DECL(move)
        }
        NGF_PY_END_DECL

        //--- Serialisation ------------------------------------------------------------
        NGF_SERIALISE_BEGIN(Crate)
        {
            GRALL2_SERIALISE_GAMEOBJECT();

            NGF_SERIALISE_OGRE(Bool, mMoving);
            NGF_SERIALISE_OGRE(Vector3, mNextDirection);
            NGF_SERIALISE_OGRE(Real, mDistanceMoved);
            NGF_SERIALISE_OGRE(Real, mSize);
        }
        NGF_SERIALISE_END
};

#ifdef __CRATE_CPP__

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
/* maximum key range = 1, duplicates = 0 */

class NGF_PY_CLASS_GPERF(Crate)
{
private:
  static inline unsigned int MakeHash (const char *str, unsigned int len);
public:
  static const struct PythonMethod *Lookup (const char *str, unsigned int len);
};

inline /*ARGSUSED*/
unsigned int
NGF_PY_CLASS_GPERF(Crate)::MakeHash (register const char *str, register unsigned int len)
{
  return len;
}

const struct PythonMethod *
NGF_PY_CLASS_GPERF(Crate)::Lookup (register const char *str, register unsigned int len)
{
  enum
    {
      TOTAL_KEYWORDS = 1,
      MIN_WORD_LENGTH = 4,
      MAX_WORD_LENGTH = 4,
      MIN_HASH_VALUE = 4,
      MAX_HASH_VALUE = 4
    };

  static const struct PythonMethod wordlist[] =
    {
      {"move", NGF_PY_METHOD_GPERF(Crate, move)}
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
