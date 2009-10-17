/*
 * =====================================================================================
 *
 *       Filename:  GraLL2GameObject.h
 *
 *    Description:  A base GameObject for all the level objects (obstacles etc.) in
 *                  GraLL 2. No instance of this is actually ever created.
 *
 *                  It provides the base Python interface for all GameObjects, holds
 *                  the main physics and ogre objects, provides a method to
 *                  serialise these, handles existence or inexistence in dimensions
 *                  etc., things common to all GameObjects.
 *
 *        Created:  04/28/2009 07:11:46 PM
 *       Compiler:  gcc
 *
 *         Author:  Nikhilesh (nikki)
 *
 * =====================================================================================
 */

#ifndef __GRALL2GAMEOBJECT_H__
#define __GRALL2GAMEOBJECT_H__

#include "Globals.h"

#define GRALL2_SERIALISE_GAMEOBJECT()                                                          \
            /* Position, rotation. */                                                          \
            if (mNode)                                                                         \
            {                                                                                  \
                NGF_SERIALISE_POSITION(mNode->getPosition());                                  \
                NGF_SERIALISE_ROTATION(mNode->getOrientation());                               \
            }                                                                                  \
                                                                                               \
            /* Complex members. */                                                             \
            if (mBody)                                                                         \
                NGF_SERIALISE_BULLET_BODY(mBody);                                              \
            NGF_SERIALISE_PYTHON_LOCALS();                                                     \
            GRALL2_SERIALISE_ALARMS();                                                         \
                                                                                               \
            /* Simple members. */                                                              \
            NGF_SERIALISE_OGRE(Int, mDimensions);


class GraLL2GameObject : 
    public NGF::Bullet::BulletGameObject,
    public NGF::Python::PythonGameObject,
    public NGF::Serialisation::SerialisableGameObject,
    public AlarmGameObject,
    public Util::ExtraEventListener
{
    protected:
        Ogre::String mOgreName; //Name for our Ogre SceneNode, Entity etc.
        Ogre::SceneNode *mNode;
        //Ogre::Entity *mEntity;

        btRigidBody *mBody;
        //btCollisionShape *mShape;

        int mDimensions;

        Ogre::Real mClickedTime; //Time after turning invisible for click feedback.

        bool mVisible;

    public:
        GraLL2GameObject(bool dimensional = true);
        virtual ~GraLL2GameObject() = 0;

        //--- Events -------------------------------------------------------------------
        virtual void unpausedTick(const Ogre::FrameEvent &evt);
        virtual void pausedTick(const Ogre::FrameEvent &evt) {}
        virtual NGF::MessageReply receiveMessage(NGF::Message msg);
        void alarm(Alarm a) { NGF_PY_CALL_EVENT(alarm, a); }
        virtual void collide(GameObject *other, btCollisionObject *otherPhysicsObject, btManifoldPoint &contact) {}

        //--- Non-NGF methods ----------------------------------------------------------
        void initBody(int specialFlags = DimensionManager::NONE);
        Ogre::Entity *createBrushEntity();
        void destroyBody() { if (mBody) { GlbVar.phyWorld->removeRigidBody(mBody); delete mBody->getMotionState(); delete mBody; mBody = 0; } }
        void setDimension(int dimension);
        int getDimensions() { return mDimensions; }
        void checkFell() { if (mBody && mBody->getWorldTransform().getOrigin().y() < -20) GlbVar.goMgr->requestDestroy(getID()); }

        //--- Python interface ---------------------------------------------------------
        virtual NGF_PY_BEGIN_DECL(GraLL2GameObject)
        {
            //setAlarm(time, alarmnum)
            NGF_PY_METHOD_DECL(setAlarm)

            //switchDimension()
            NGF_PY_METHOD_DECL(switchDimension)
            //setDimension(dimension)
            NGF_PY_METHOD_DECL(setDimension)

            //getPosition()
            NGF_PY_METHOD_DECL(getPosition)
            //setPosition(pos)
            NGF_PY_METHOD_DECL(setPosition)
            //translate(disp)
            NGF_PY_METHOD_DECL(translate)
            //getOrientation()
            NGF_PY_METHOD_DECL(getOrientation)
            //setOrientation(rot)
            NGF_PY_METHOD_DECL(setOrientation)

            //applyForce(force, offset)
            NGF_PY_METHOD_DECL(applyForce)
            //applyCentralForce(force)
            NGF_PY_METHOD_DECL(applyCentralForce)
            //getTotalForce()
            NGF_PY_METHOD_DECL(getTotalForce)
            //applyTorque(torque)
            NGF_PY_METHOD_DECL(applyTorque)
            //applyTorqueImpulse(impulse)
            NGF_PY_METHOD_DECL(applyTorqueImpulse)
            //getTotalTorque()
            NGF_PY_METHOD_DECL(getTotalTorque)

            //applyImpulse(impulse, offset)
            NGF_PY_METHOD_DECL(applyImpulse)
            //applyCentralImpulse(impulse)
            NGF_PY_METHOD_DECL(applyCentralImpulse)
            //setLinearVelocity(velocity)
            NGF_PY_METHOD_DECL(setLinearVelocity)
            //getLinearVelocity()
            NGF_PY_METHOD_DECL(getLinearVelocity)
            //getVelocityInLocalPoint(offset)
            NGF_PY_METHOD_DECL(getVelocityInLocalPoint)
            //setAngularVelocity(velocity)
            NGF_PY_METHOD_DECL(setAngularVelocity)
            //getAngularVelocity()
            NGF_PY_METHOD_DECL(getAngularVelocity)

            NGF_PY_METHOD_DECL(setVisible)
        }
        NGF_PY_END_DECL

        //--- Serialisation ------------------------------------------------------------
        virtual NGF_SERIALISE_BEGIN(GraLL2GameObject)
        {
            GRALL2_SERIALISE_GAMEOBJECT();
        }
        NGF_SERIALISE_END
};

#ifdef  __GRALL2GAMEOBJECT_CPP__

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
/* maximum key range = 30, duplicates = 0 */

class NGF_PY_CLASS_GPERF(GraLL2GameObject)
{
private:
  static inline unsigned int MakeHash (const char *str, unsigned int len);
public:
  static const struct PythonMethod *Lookup (const char *str, unsigned int len);
};

inline unsigned int
NGF_PY_CLASS_GPERF(GraLL2GameObject)::MakeHash (register const char *str, register unsigned int len)
{
  static const unsigned char asso_values[] =
    {
      40, 40, 40, 40, 40, 40, 40, 40, 40, 40,
      40, 40, 40, 40, 40, 40, 40, 40, 40, 40,
      40, 40, 40, 40, 40, 40, 40, 40, 40, 40,
      40, 40, 40, 40, 40, 40, 40, 40, 40, 40,
      40, 40, 40, 40, 40, 40, 40, 40, 40, 40,
      40, 40, 40, 40, 40, 40, 40, 40, 40, 40,
      40, 40, 40, 40, 40,  5, 40, 40, 20, 40,
      40, 40, 40, 40, 40, 40,  5, 40, 40, 10,
       5, 40, 40, 40, 20, 40,  5, 40, 40, 40,
      40, 40, 40, 40, 40, 40, 40,  0, 40, 40,
      40, 40, 40,  5, 40, 40, 40, 40,  0, 40,
       0, 40, 40, 40, 40,  0,  5, 40, 40, 40,
      40, 40, 40, 40, 40, 40, 40, 40, 40, 40,
      40, 40, 40, 40, 40, 40, 40, 40, 40, 40,
      40, 40, 40, 40, 40, 40, 40, 40, 40, 40,
      40, 40, 40, 40, 40, 40, 40, 40, 40, 40,
      40, 40, 40, 40, 40, 40, 40, 40, 40, 40,
      40, 40, 40, 40, 40, 40, 40, 40, 40, 40,
      40, 40, 40, 40, 40, 40, 40, 40, 40, 40,
      40, 40, 40, 40, 40, 40, 40, 40, 40, 40,
      40, 40, 40, 40, 40, 40, 40, 40, 40, 40,
      40, 40, 40, 40, 40, 40, 40, 40, 40, 40,
      40, 40, 40, 40, 40, 40, 40, 40, 40, 40,
      40, 40, 40, 40, 40, 40, 40, 40, 40, 40,
      40, 40, 40, 40, 40, 40, 40, 40, 40, 40,
      40, 40, 40, 40, 40, 40
    };
  return len + asso_values[(unsigned char)str[3]] + asso_values[(unsigned char)str[0]];
}

const struct PythonMethod *
NGF_PY_CLASS_GPERF(GraLL2GameObject)::Lookup (register const char *str, register unsigned int len)
{
  enum
    {
      TOTAL_KEYWORDS = 22,
      MIN_WORD_LENGTH = 8,
      MAX_WORD_LENGTH = 23,
      MIN_HASH_VALUE = 10,
      MAX_HASH_VALUE = 39
    };

  static const struct PythonMethod wordlist[] =
    {
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""},
      {"applyForce", NGF_PY_METHOD_GPERF(GraLL2GameObject, applyForce)},
      {"applyTorque", NGF_PY_METHOD_GPERF(GraLL2GameObject, applyTorque)},
      {"applyImpulse", NGF_PY_METHOD_GPERF(GraLL2GameObject, applyImpulse)},
      {"setAlarm", NGF_PY_METHOD_GPERF(GraLL2GameObject, setAlarm)},
      {"translate", NGF_PY_METHOD_GPERF(GraLL2GameObject, translate)},
      {"setVisible", NGF_PY_METHOD_GPERF(GraLL2GameObject, setVisible)},
      {"setPosition", NGF_PY_METHOD_GPERF(GraLL2GameObject, setPosition)},
      {"applyCentralForce", NGF_PY_METHOD_GPERF(GraLL2GameObject, applyCentralForce)},
      {"applyTorqueImpulse", NGF_PY_METHOD_GPERF(GraLL2GameObject, applyTorqueImpulse)},
      {"applyCentralImpulse", NGF_PY_METHOD_GPERF(GraLL2GameObject, applyCentralImpulse)},
      {"switchDimension", NGF_PY_METHOD_GPERF(GraLL2GameObject, switchDimension)},
      {"getPosition", NGF_PY_METHOD_GPERF(GraLL2GameObject, getPosition)},
      {"setLinearVelocity", NGF_PY_METHOD_GPERF(GraLL2GameObject, setLinearVelocity)},
      {"setAngularVelocity", NGF_PY_METHOD_GPERF(GraLL2GameObject, setAngularVelocity)},
      {"setOrientation", NGF_PY_METHOD_GPERF(GraLL2GameObject, setOrientation)},
      {""}, {""},
      {"getLinearVelocity", NGF_PY_METHOD_GPERF(GraLL2GameObject, getLinearVelocity)},
      {"getAngularVelocity", NGF_PY_METHOD_GPERF(GraLL2GameObject, getAngularVelocity)},
      {"getOrientation", NGF_PY_METHOD_GPERF(GraLL2GameObject, getOrientation)},
      {""}, {""},
      {"setDimension", NGF_PY_METHOD_GPERF(GraLL2GameObject, setDimension)},
      {"getVelocityInLocalPoint", NGF_PY_METHOD_GPERF(GraLL2GameObject, getVelocityInLocalPoint)},
      {""}, {""}, {""}, {""},
      {"getTotalForce", NGF_PY_METHOD_GPERF(GraLL2GameObject, getTotalForce)},
      {"getTotalTorque", NGF_PY_METHOD_GPERF(GraLL2GameObject, getTotalTorque)}
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
