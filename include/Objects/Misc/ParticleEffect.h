/*
 * =====================================================================================
 *
 *       Filename:  ParticleEffect.h
 *
 *    Description:  A particle effect.
 *
 *        Created:  06/15/2009 09:31:11 PM
 *       Compiler:  gcc
 *
 *         Author:  Nikhilesh  (nikki)
 *
 * =====================================================================================
 */

#ifndef __PARTICLEEFFECT_H__
#define __PARTICLEEFFECT_H__

#include "Globals.h"

//NOTE: If you're making this Python scriptable, just derive from NGF::PythonGameObject too,
//      add the 'NGF_PYTHON_DECL' and 'NGF_PYTHON_IMPL', and do the gperf stuff. Check
//      GraLL2GameObjects for guidance.

class ParticleEffect :
    public NGF::Serialisation::SerialisableGameObject
{
    protected:
        Ogre::SceneNode *mNode;
        Ogre::ParticleSystem *mParticleSystem;

        bool mTimed;
        Ogre::Real mTimeLeft;

    public:
        ParticleEffect(Ogre::Vector3 pos, Ogre::Quaternion rot, NGF::ID id, NGF::PropertyList properties, Ogre::String name);
        virtual ~ParticleEffect();

        //--- Events -------------------------------------------------------------------
        void unpausedTick(const Ogre::FrameEvent &evt);

        void pausedTick(const Ogre::FrameEvent &evt) {}
        NGF::MessageReply receiveMessage(NGF::Message msg) { NGF_NO_REPLY(); }

        //--- Serialisation ------------------------------------------------------------
        NGF_SERIALISE_BEGIN(ParticleEffect)
        {
            NGF_SERIALISE_POSITION(mNode->getPosition());
            NGF_SERIALISE_ROTATION(mNode->getOrientation());

            NGF_SERIALISE_OGRE(Bool, mTimed);
            NGF_SERIALISE_OGRE(Real, mTimeLeft);
        }
        NGF_SERIALISE_END
};

#endif
