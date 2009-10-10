/*
 * =====================================================================================
 *
 *       Filename:  FallingBrush.h
 *
 *    Description:  Falls after you roll over it.
 *
 *        Created:  10/10/2009 03:44:12 PM
 *       Compiler:  gcc
 *
 *         Author:  Nikhilesh S (nikki)
 *
 * =====================================================================================
 */

#ifndef __FALLINGBRUSH_H__
#define __FALLINGBRUSH_H__

#include "Globals.h"
#include "Objects/Main/GraLL2GameObject.h"

class FallingBrush :
    public GraLL2GameObject
{
    protected:
        btCollisionShape *mShape;
        Ogre::Entity *mEntity;

        Ogre::Real mTime;
        Ogre::Real mFallTime;
        bool mFell;

    public:
        FallingBrush(Ogre::Vector3 pos, Ogre::Quaternion rot, NGF::ID id, NGF::PropertyList properties, Ogre::String name);
        virtual ~FallingBrush();

        //--- Events -------------------------------------------------------------------
        void postLoad();
        void unpausedTick(const Ogre::FrameEvent &evt);
        void pausedTick(const Ogre::FrameEvent &evt);
        NGF::MessageReply receiveMessage(NGF::Message msg);
        void collide(GameObject *other, btCollisionObject *otherPhysicsObject, btManifoldPoint &contact);

        //--- Non-NGF ------------------------------------------------------------------
        void fall();

        //--- Python interface ---------------------------------------------------------
        /*
        NGF_PY_BEGIN_DECL(FallingBrush)
        {
        }
        NGF_PY_END_DECL
        */

        //--- Serialisation ------------------------------------------------------------
        NGF_SERIALISE_BEGIN(FallingBrush)
        {
            GRALL2_SERIALISE_GAMEOBJECT();

            NGF_SERIALISE_OGRE(Real, mTime);
            NGF_SERIALISE_OGRE(Real, mFallTime);
            NGF_SERIALISE_OGRE(Bool, mFell);
        }
        NGF_SERIALISE_END
};

#ifdef __FALLINGBRUSH_CPP__


#endif

#endif
