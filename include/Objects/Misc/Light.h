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

    public:
        Light(Ogre::Vector3 pos, Ogre::Quaternion rot, NGF::ID id, NGF::PropertyList properties, Ogre::String name);
        virtual ~Light();

        //--- Events -------------------------------------------------------------------
        void unpausedTick(const Ogre::FrameEvent &evt) {}
        void pausedTick(const Ogre::FrameEvent &evt) {}
        NGF::MessageReply receiveMessage(NGF::Message msg) { NGF_NO_REPLY(); }

        //--- Serialisation ------------------------------------------------------------
        NGF_SERIALISE_BEGIN(Light)
        {
            NGF_SERIALISE_POSITION(mNode->getPosition());
            NGF_SERIALISE_ROTATION(mNode->getPosition());
        }
        NGF_SERIALISE_END
};

#endif
