/*
 * =====================================================================================
 *
 *       Filename:  Light.h
 *
 *    Description:  A 'Light' GameObject allowing lights to fit easily into the 
 *                  GameObject idea. It's not a GraLL2GameObject since it's not a
 *                  phyisics object.
 *
 *        Created:  05/10/2009 09:06:58 PM
 *       Compiler:  gcc
 *
 *         Author:  Nikhilesh (nikki)
 *
 * =====================================================================================
 */

#ifndef __LIGHT_H__
#define __LIGHT_H__

#include "Globals.h"

//NOTE: If you're making this Python scriptable, just derive from NGF::PythonGameObject too,
//      add the 'NGF_PYTHON_DECL' and 'NGF_PYTHON_IMPL', and do the gperf stuff. Check
//      GraLL2GameObjects for guidance.

class Light :
    public NGF::Serialisation::SerialisableGameObject,
    public NGF::Python::PythonGameObject
{
    protected:
        Ogre::SceneNode *mNode;
        Ogre::Light *mLight;

        bool mTimed;
        Ogre::Real mTimeLeft;
        Ogre::Real mFadeOutTime;
        
        Ogre::ColourValue mDiffuseColour;
        Ogre::ColourValue mSpecularColour;

        bool mOn;

    public:
        Light(Ogre::Vector3 pos, Ogre::Quaternion rot, NGF::ID id, NGF::PropertyList properties, Ogre::String name);
        virtual ~Light();

        //--- Events -------------------------------------------------------------------
        void unpausedTick(const Ogre::FrameEvent &evt); //{ mNode->translate(Ogre::Vector3(Ogre::Math::Sin(mTime),0,Ogre::Math::Cos(mTime)) * evt.timeSinceLastFrame); mTime += evt.timeSinceLastFrame; }

        void pausedTick(const Ogre::FrameEvent &evt) {}
        NGF::MessageReply receiveMessage(NGF::Message msg);

        //--- Python interface ---------------------------------------------------------
        NGF_PY_BEGIN_DECL(Light)
        {
            NGF_PY_METHOD_DECL(setDiffuseColour)
            NGF_PY_METHOD_DECL(setSpecularColour)
            NGF_PY_METHOD_DECL(setAttenuation)
            NGF_PY_METHOD_DECL(setInnerConeAngle)
            NGF_PY_METHOD_DECL(setOuterConeAngle)
        }
        NGF_PY_END_DECL

        //--- Serialisation ------------------------------------------------------------
        NGF_SERIALISE_BEGIN(Light)
        {
            Ogre::String diffuse, specular;

            NGF_SERIALISE_ON_SAVE
            {
                diffuse = Ogre::StringConverter::toString(mLight->getDiffuseColour());
                specular = Ogre::StringConverter::toString(mLight->getSpecularColour());
            }

            NGF_SERIALISE_POSITION(mNode->getPosition());
            NGF_SERIALISE_ROTATION(mNode->getOrientation());
            
            NGF_SERIALISE_OGRE(Bool, mTimed);
            NGF_SERIALISE_OGRE(Real, mTimeLeft);
            NGF_SERIALISE_OGRE(Real, mFadeOutTime);

            NGF_SERIALISE_STRING(diffuse);
            NGF_SERIALISE_STRING(specular);

            NGF_SERIALISE_ON_LOAD
            {
                mLight->setDiffuseColour(Ogre::StringConverter::parseColourValue(diffuse));
                mLight->setSpecularColour(Ogre::StringConverter::parseColourValue(specular));
            }
        }
        NGF_SERIALISE_END
};

#ifdef __LIGHT_CPP__

/* C++ code produced by gperf version 3.0.4 *//*{{{*/
/* Command-line: gperf  */
/* Computed positions: -k'4' */

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
/* maximum key range = 9, duplicates = 0 */

class NGF_PY_CLASS_GPERF(Light)
{
private:
  static inline unsigned int MakeHash (const char *str, unsigned int len);
public:
  static const struct PythonMethod *Lookup (const char *str, unsigned int len);
};

inline unsigned int
NGF_PY_CLASS_GPERF(Light)::MakeHash (register const char *str, register unsigned int len)
{
  static const unsigned char asso_values[] =
    {
      23, 23, 23, 23, 23, 23, 23, 23, 23, 23,
      23, 23, 23, 23, 23, 23, 23, 23, 23, 23,
      23, 23, 23, 23, 23, 23, 23, 23, 23, 23,
      23, 23, 23, 23, 23, 23, 23, 23, 23, 23,
      23, 23, 23, 23, 23, 23, 23, 23, 23, 23,
      23, 23, 23, 23, 23, 23, 23, 23, 23, 23,
      23, 23, 23, 23, 23,  0, 23, 23,  0, 23,
      23, 23, 23,  2, 23, 23, 23, 23, 23,  5,
      23, 23, 23,  0, 23, 23, 23, 23, 23, 23,
      23, 23, 23, 23, 23, 23, 23, 23, 23, 23,
      23, 23, 23, 23, 23, 23, 23, 23, 23, 23,
      23, 23, 23, 23, 23, 23, 23, 23, 23, 23,
      23, 23, 23, 23, 23, 23, 23, 23, 23, 23,
      23, 23, 23, 23, 23, 23, 23, 23, 23, 23,
      23, 23, 23, 23, 23, 23, 23, 23, 23, 23,
      23, 23, 23, 23, 23, 23, 23, 23, 23, 23,
      23, 23, 23, 23, 23, 23, 23, 23, 23, 23,
      23, 23, 23, 23, 23, 23, 23, 23, 23, 23,
      23, 23, 23, 23, 23, 23, 23, 23, 23, 23,
      23, 23, 23, 23, 23, 23, 23, 23, 23, 23,
      23, 23, 23, 23, 23, 23, 23, 23, 23, 23,
      23, 23, 23, 23, 23, 23, 23, 23, 23, 23,
      23, 23, 23, 23, 23, 23, 23, 23, 23, 23,
      23, 23, 23, 23, 23, 23, 23, 23, 23, 23,
      23, 23, 23, 23, 23, 23, 23, 23, 23, 23,
      23, 23, 23, 23, 23, 23
    };
  return len + asso_values[(unsigned char)str[3]];
}

const struct PythonMethod *
NGF_PY_CLASS_GPERF(Light)::Lookup (register const char *str, register unsigned int len)
{
  enum
    {
      TOTAL_KEYWORDS = 5,
      MIN_WORD_LENGTH = 14,
      MAX_WORD_LENGTH = 17,
      MIN_HASH_VALUE = 14,
      MAX_HASH_VALUE = 22
    };

  static const struct PythonMethod wordlist[] =
    {
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""},
      {"setAttenuation", NGF_PY_METHOD_GPERF(Light, setAttenuation)},
      {""},
      {"setDiffuseColour", NGF_PY_METHOD_GPERF(Light, setDiffuseColour)},
      {"setSpecularColour", NGF_PY_METHOD_GPERF(Light, setSpecularColour)},
      {""},
      {"setInnerConeAngle", NGF_PY_METHOD_GPERF(Light, setInnerConeAngle)},
      {""}, {""},
      {"setOuterConeAngle", NGF_PY_METHOD_GPERF(Light, setOuterConeAngle)}
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
