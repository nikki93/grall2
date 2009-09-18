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

        Ogre::Real mFireTime;
        Ogre::Real mRestTime;

        enum
        {
            TS_FIRETOREST,
            TS_REST,
            TS_RESTTOFIRE,
            TS_FIRE,
        };

        int mState;

        Ogre::Real mStateTimer;
        Ogre::Real mBulletTimer;

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
        void fire();
        void rest();
        void fireSingleBullet();

        //--- Python interface ---------------------------------------------------------
        NGF_PY_BEGIN_DECL(Turret)
        {
            NGF_PY_METHOD_DECL(startFiring)
            NGF_PY_METHOD_DECL(stopFiring)
        }
        NGF_PY_END_DECL

        //--- Serialisation ------------------------------------------------------------
        NGF_SERIALISE_BEGIN(Turret)
        {
            Ogre::Vector3 topPos;

            NGF_SERIALISE_ON_SAVE
            {
                topPos = mTopNode->getPosition();
            }

            GRALL2_SERIALISE_GAMEOBJECT();

            NGF_SERIALISE_OGRE(Real, mFireTime);
            NGF_SERIALISE_OGRE(Real, mRestTime);

            NGF_SERIALISE_OGRE(Int, mState);

            NGF_SERIALISE_OGRE(Real, mStateTimer);
            NGF_SERIALISE_OGRE(Real, mBulletTimer);

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

class NGF_PY_CLASS_GPERF(Turret)
{
private:
  static inline unsigned int MakeHash (const char *str, unsigned int len);
public:
  static const struct PythonMethod *Lookup (const char *str, unsigned int len);
};

inline /*ARGSUSED*/
unsigned int
NGF_PY_CLASS_GPERF(Turret)::MakeHash (register const char *str, register unsigned int len)
{
  return len;
}

const struct PythonMethod *
NGF_PY_CLASS_GPERF(Turret)::Lookup (register const char *str, register unsigned int len)
{
  enum
    {
      TOTAL_KEYWORDS = 2,
      MIN_WORD_LENGTH = 10,
      MAX_WORD_LENGTH = 11,
      MIN_HASH_VALUE = 10,
      MAX_HASH_VALUE = 11
    };

  static const struct PythonMethod wordlist[] =
    {
      {"stopFiring", NGF_PY_METHOD_GPERF(Turret, stopFiring)},
      {"startFiring", NGF_PY_METHOD_GPERF(Turret, startFiring)}
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
