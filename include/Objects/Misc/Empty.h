/*
 * =====================================================================================
 *
 *       Filename:  Empty.h
 *
 *    Description:  An 'empty' object. Does nothing, but very Python-scriptable, so good
 *                  for making 'from-scratch' types in Python.
 *
 *        Created:  10/05/2009 09:46:19 PM
 *       Compiler:  gcc
 *
 *         Author:  Nikhilesh S (nikki)
 *
 * =====================================================================================
 */

#ifndef __EMPTY_H__
#define __EMPTY_H__

#include "Globals.h"
#include "Objects/Main/GraLL2GameObject.h"

class Empty :
    public GraLL2GameObject
{
    protected:
        Ogre::Entity *mEntity;
        btCollisionShape *mShape;

        //For temporary storage until creation.
        Ogre::Vector3 mPos;
        Ogre::Quaternion mRot;

        Ogre::Real mp_Mass;
        Ogre::Real mp_Friction;
        Ogre::Real mp_Restitution;
        Ogre::Real mp_LinearDamping;
        Ogre::Real mp_AngularDamping;

    public:
        Empty(Ogre::Vector3 pos, Ogre::Quaternion rot, NGF::ID id, NGF::PropertyList properties, Ogre::String name);
        virtual ~Empty();

        //--- Events -------------------------------------------------------------------
        void postLoad();
        void unpausedTick(const Ogre::FrameEvent &evt);
        void pausedTick(const Ogre::FrameEvent &evt);
        NGF::MessageReply receiveMessage(NGF::Message msg);
        void collide(GameObject *other, btCollisionObject *otherPhysicsObject, btManifoldPoint &contact);

        //--- Non-NGF ------------------------------------------------------------------
        void createBody(int shape, int bodyType, int flags);
        void createMesh(Ogre::String meshName);
        void createBrushMesh();
        void setMaterial(Ogre::String matName);

        //--- Python interface ---------------------------------------------------------
        NGF_PY_BEGIN_DECL(Empty)
        {
            NGF_PY_METHOD_DECL(createBody)
            NGF_PY_METHOD_DECL(createMesh)
            NGF_PY_METHOD_DECL(createBrushMesh)
            NGF_PY_METHOD_DECL(setMaterial)
            NGF_PY_METHOD_DECL(getPosition)
            NGF_PY_METHOD_DECL(getOrientation)

            NGF_PY_PROPERTY_DECL(mass)
            NGF_PY_PROPERTY_DECL(friction)
            NGF_PY_PROPERTY_DECL(restitution)
            NGF_PY_PROPERTY_DECL(linearDamping)
            NGF_PY_PROPERTY_DECL(angularDamping)
        }
        NGF_PY_END_DECL

        //--- Serialisation ------------------------------------------------------------
        NGF_SERIALISE_BEGIN(Empty)
        {
            GRALL2_SERIALISE_GAMEOBJECT();

            //GraLL2GameObject will skip if no SceneNode, so do it.
            if (!mNode)
            {
                NGF_SERIALISE_POSITION(mPos);
                NGF_SERIALISE_ROTATION(mRot);
            }

            NGF_SERIALISE_OGRE(Real, mp_Mass);
            NGF_SERIALISE_OGRE(Real, mp_Friction);
            NGF_SERIALISE_OGRE(Real, mp_Restitution);
            NGF_SERIALISE_OGRE(Real, mp_LinearDamping);
            NGF_SERIALISE_OGRE(Real, mp_AngularDamping);
        }
        NGF_SERIALISE_END
};

#ifdef __EMPTY_CPP__

/* C++ code produced by gperf version 3.0.4 *//*{{{*/
/* Command-line: gperf  */
/* Computed positions: -k'1,7' */

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
/* maximum key range = 28, duplicates = 0 */

class NGF_PY_CLASS_GPERF(Empty)
{
private:
  static inline unsigned int MakeHash (const char *str, unsigned int len);
public:
  static const struct PythonMethod *Lookup (const char *str, unsigned int len);
};

inline unsigned int
NGF_PY_CLASS_GPERF(Empty)::MakeHash (register const char *str, register unsigned int len)
{
  static const unsigned char asso_values[] =
    {
      36, 36, 36, 36, 36, 36, 36, 36, 36, 36,
      36, 36, 36, 36, 36, 36, 36, 36, 36, 36,
      36, 36, 36, 36, 36, 36, 36, 36, 36, 36,
      36, 36, 36, 36, 36, 36, 36, 36, 36, 36,
      36, 36, 36, 36, 36, 36, 36, 36, 36, 36,
      36, 36, 36, 36, 36, 36, 36, 36, 36, 36,
      36, 36, 36, 36, 36, 36, 10, 36, 36, 36,
      36, 36, 36, 36, 36, 36, 36, 20, 36, 36,
      36, 36, 36, 36, 36, 36, 36, 36, 36, 36,
      36, 36, 36, 36, 36, 36, 36, 36, 36,  5,
      36,  0, 36,  5, 36,  0, 36, 36, 36, 36,
       5, 36, 36, 36, 36,  0, 36, 36, 36, 36,
      36, 36, 36, 36, 36, 36, 36, 36, 36, 36,
      36, 36, 36, 36, 36, 36, 36, 36, 36, 36,
      36, 36, 36, 36, 36, 36, 36, 36, 36, 36,
      36, 36, 36, 36, 36, 36, 36, 36, 36, 36,
      36, 36, 36, 36, 36, 36, 36, 36, 36, 36,
      36, 36, 36, 36, 36, 36, 36, 36, 36, 36,
      36, 36, 36, 36, 36, 36, 36, 36, 36, 36,
      36, 36, 36, 36, 36, 36, 36, 36, 36, 36,
      36, 36, 36, 36, 36, 36, 36, 36, 36, 36,
      36, 36, 36, 36, 36, 36, 36, 36, 36, 36,
      36, 36, 36, 36, 36, 36, 36, 36, 36, 36,
      36, 36, 36, 36, 36, 36, 36, 36, 36, 36,
      36, 36, 36, 36, 36, 36, 36, 36, 36, 36,
      36, 36, 36, 36, 36, 36
    };
  return len + asso_values[(unsigned char)str[6]] + asso_values[(unsigned char)str[0]];
}

const struct PythonMethod *
NGF_PY_CLASS_GPERF(Empty)::Lookup (register const char *str, register unsigned int len)
{
  enum
    {
      TOTAL_KEYWORDS = 16,
      MIN_WORD_LENGTH = 8,
      MAX_WORD_LENGTH = 18,
      MIN_HASH_VALUE = 8,
      MAX_HASH_VALUE = 35
    };

  static const struct PythonMethod wordlist[] =
    {
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {"set_mass", NGF_PY_SET_GPERF(Empty, mass)},
      {""}, {""},
      {"setMaterial", NGF_PY_METHOD_GPERF(Empty, setMaterial)},
      {"set_friction", NGF_PY_SET_GPERF(Empty, friction)},
      {"get_mass", NGF_PY_GET_GPERF(Empty, mass)},
      {""},
      {"set_restitution", NGF_PY_SET_GPERF(Empty, restitution)},
      {"getPosition", NGF_PY_METHOD_GPERF(Empty, getPosition)},
      {"get_friction", NGF_PY_GET_GPERF(Empty, friction)},
      {""},
      {"getOrientation", NGF_PY_METHOD_GPERF(Empty, getOrientation)},
      {"get_restitution", NGF_PY_GET_GPERF(Empty, restitution)},
      {""},
      {"set_linearDamping", NGF_PY_SET_GPERF(Empty, linearDamping)},
      {"set_angularDamping", NGF_PY_SET_GPERF(Empty, angularDamping)},
      {""},
      {"createBody", NGF_PY_METHOD_GPERF(Empty, createBody)},
      {""},
      {"get_linearDamping", NGF_PY_GET_GPERF(Empty, linearDamping)},
      {"get_angularDamping", NGF_PY_GET_GPERF(Empty, angularDamping)},
      {""},
      {"createBrushMesh", NGF_PY_METHOD_GPERF(Empty, createBrushMesh)},
      {""}, {""}, {""}, {""},
      {"createMesh", NGF_PY_METHOD_GPERF(Empty, createMesh)}
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
