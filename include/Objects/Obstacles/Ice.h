/*
 * =====================================================================================
 *
 *       Filename:  Ice.h
 *
 *    Description:  Ice. Slippery stuff! :-)
 *
 *        Created:  06/16/2009 08:41:27 PM
 *       Compiler:  gcc
 *
 *         Author:  Nikhilesh (nikki)
 *
 * =====================================================================================
 */

#ifndef __ICE_H__
#define __ICE_H__

#include "Globals.h"
#include "Objects/Main/GraLL2GameObject.h"

class Ice :
    public GraLL2GameObject
{
    protected:
        btCollisionShape *mShape;
        Ogre::Entity *mEntity;

        Ogre::Real mFrictionCoeff;

    public:
        Ice(Ogre::Vector3 pos, Ogre::Quaternion rot, NGF::ID id, NGF::PropertyList properties, Ogre::String name);
        virtual ~Ice();

        //--- Events -------------------------------------------------------------------
        void postLoad();
        void unpausedTick(const Ogre::FrameEvent &evt);
        void pausedTick(const Ogre::FrameEvent &evt);
        NGF::MessageReply receiveMessage(NGF::Message msg);
        void collide(GameObject *other, btCollisionObject *otherPhysicsObject, btManifoldPoint &contact);

        //--- Python interface ---------------------------------------------------------
        /*
        NGF_PY_BEGIN_DECL(Ice)
        {
        }
        NGF_PY_END_DECL
        */

        //--- Serialisation ------------------------------------------------------------
        NGF_SERIALISE_BEGIN(Ice)
        {
            GRALL2_SERIALISE_GAMEOBJECT();

            NGF_SERIALISE_OGRE(Real, mFrictionCoeff);
        }
        NGF_SERIALISE_END
};

#ifdef __ICE_CPP__

//gperf stuff goes here.

#endif

#endif
