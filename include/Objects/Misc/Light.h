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
    public NGF::Serialisation::SerialisableGameObject
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

#endif
