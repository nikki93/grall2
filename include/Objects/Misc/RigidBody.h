/*
 * =====================================================================================
 *
 *       Filename:  RigidBody.h
 *
 *    Description:  A generic 'RigidBody' brush. Just like StaticBrush, except that now
 *                  the bodies can move.
 *
 *        Created:  05/28/2009 06:48:32 PM
 *       Compiler:  gcc
 *
 *         Author:  Nikhilesh (nikki)
 *
 * =====================================================================================
 */

#ifndef __RIGIDBODY_H__
#define __RIGIDBODY_H__

#include "Globals.h"
#include "Objects/Main/GraLL2GameObject.h"

class RigidBody :
    public GraLL2GameObject
{
    protected:
        btCollisionShape *mShape;
        Ogre::Entity *mEntity;

        Ogre::Real mp_Mass;
        Ogre::Real mp_Friction;
        Ogre::Real mp_Restitution;
        Ogre::Real mp_LinearDamping;
        Ogre::Real mp_AngularDamping;

    public:
        RigidBody(Ogre::Vector3 pos, Ogre::Quaternion rot, NGF::ID id, NGF::PropertyList properties, Ogre::String name);
        virtual ~RigidBody();

        //--- Events -------------------------------------------------------------------
        void postLoad();
        void unpausedTick(const Ogre::FrameEvent &evt);
        void pausedTick(const Ogre::FrameEvent &evt);
        NGF::MessageReply receiveMessage(NGF::Message msg);
        void collide(GameObject *other, btCollisionObject *otherPhysicsObject, btManifoldPoint &contact);

        //--- Python interface ---------------------------------------------------------
        NGF_PY_BEGIN_DECL(RigidBody)
        {
            NGF_PY_PROPERTY_DECL(mass)
            NGF_PY_PROPERTY_DECL(friction)
            NGF_PY_PROPERTY_DECL(restitution)
            NGF_PY_PROPERTY_DECL(linearDamping)
            NGF_PY_PROPERTY_DECL(angularDamping)
        }
        NGF_PY_END_DECL

        //--- Serialisation ------------------------------------------------------------
        NGF_SERIALISE_BEGIN(RigidBody)
        {
            GRALL2_SERIALISE_GAMEOBJECT();

            NGF_SERIALISE_OGRE(Real, mp_Mass);
            NGF_SERIALISE_OGRE(Real, mp_Friction);
            NGF_SERIALISE_OGRE(Real, mp_Restitution);
            NGF_SERIALISE_OGRE(Real, mp_LinearDamping);
            NGF_SERIALISE_OGRE(Real, mp_AngularDamping);
        }
        NGF_SERIALISE_END
};

#ifdef __RIGIDBODY_CPP__

/* C++ code produced by gperf version 3.0.3 *//*{{{*/
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
/* maximum key range = 26, duplicates = 0 */

class NGF_PY_CLASS_GPERF(RigidBody)
{
private:
  static inline unsigned int MakeHash (const char *str, unsigned int len);
public:
  static const struct PythonMethod *Lookup (const char *str, unsigned int len);
};

inline unsigned int
NGF_PY_CLASS_GPERF(RigidBody)::MakeHash (register const char *str, register unsigned int len)
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
      34, 34, 34, 34, 34, 34, 34, 34, 34, 34,
      34, 34, 34, 15, 34, 34, 34, 34, 34, 34,
      34, 34, 34, 34, 34,  0, 34, 34, 34, 34,
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
  return len + asso_values[(unsigned char)str[0]];
}

const struct PythonMethod *
NGF_PY_CLASS_GPERF(RigidBody)::Lookup (register const char *str, register unsigned int len)
{
  enum
    {
      TOTAL_KEYWORDS = 10,
      MIN_WORD_LENGTH = 8,
      MAX_WORD_LENGTH = 18,
      MIN_HASH_VALUE = 8,
      MAX_HASH_VALUE = 33
    };

  static const struct PythonMethod wordlist[] =
    {
      {"set_mass", NGF_PY_SET_GPERF(RigidBody, mass)},
      {"set_friction", NGF_PY_SET_GPERF(RigidBody, friction)},
      {"set_restitution", NGF_PY_SET_GPERF(RigidBody, restitution)},
      {"set_linearDamping", NGF_PY_SET_GPERF(RigidBody, linearDamping)},
      {"set_angularDamping", NGF_PY_SET_GPERF(RigidBody, angularDamping)},
      {"get_mass", NGF_PY_GET_GPERF(RigidBody, mass)},
      {"get_friction", NGF_PY_GET_GPERF(RigidBody, friction)},
      {"get_restitution", NGF_PY_GET_GPERF(RigidBody, restitution)},
      {"get_linearDamping", NGF_PY_GET_GPERF(RigidBody, linearDamping)},
      {"get_angularDamping", NGF_PY_GET_GPERF(RigidBody, angularDamping)}
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
              case 4:
                resword = &wordlist[1];
                goto compare;
              case 7:
                resword = &wordlist[2];
                goto compare;
              case 9:
                resword = &wordlist[3];
                goto compare;
              case 10:
                resword = &wordlist[4];
                goto compare;
              case 15:
                resword = &wordlist[5];
                goto compare;
              case 19:
                resword = &wordlist[6];
                goto compare;
              case 22:
                resword = &wordlist[7];
                goto compare;
              case 24:
                resword = &wordlist[8];
                goto compare;
              case 25:
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
