/*
 * =====================================================================================
 *
 *       Filename:  Crate.h
 *
 *    Description:  A block you can push around.
 *
 *        Created:  06/05/2009 08:29:49 PM
 *       Compiler:  gcc
 *
 *         Author:  Nikhilesh (nikki)
 *
 * =====================================================================================
 */

#ifndef __CRATE_H__
#define __CRATE_H__

#include "Globals.h"
#include "Objects/Main/GraLL2GameObject.h"

class Crate :
    public GraLL2GameObject
{
    protected:
        btBoxShape *mShape;
        Ogre::Entity *mEntity;
        btBoxShape *mCastShape;

        bool mMoving;
        Ogre::Vector3 mNextDirection;
        Ogre::Real mDistanceMoved;
        Ogre::Real mSize;

    public:
        Crate(Ogre::Vector3 pos, Ogre::Quaternion rot, NGF::ID id, NGF::PropertyList properties, Ogre::String name);
        virtual ~Crate();

        //--- Events -------------------------------------------------------------------
        void postLoad();
        void unpausedTick(const Ogre::FrameEvent &evt);
        void pausedTick(const Ogre::FrameEvent &evt);
        NGF::MessageReply receiveMessage(NGF::Message msg);
        void collide(GameObject *other, btCollisionObject *otherPhysicsObject, btManifoldPoint &contact);

        //--- Python interface ---------------------------------------------------------
        /*
        NGF_PY_BEGIN_DECL(Crate)
        {
        }
        NGF_PY_END_DECL
        */

        //--- Serialisation ------------------------------------------------------------
        NGF_SERIALISE_BEGIN(Crate)
        {
            GRALL2_SERIALISE_GAMEOBJECT();

            NGF_SERIALISE_OGRE(Bool, mMoving);
            NGF_SERIALISE_OGRE(Vector3, mNextDirection);
            NGF_SERIALISE_OGRE(Real, mDistanceMoved);
            NGF_SERIALISE_OGRE(Real, mSize);
        }
        NGF_SERIALISE_END
};

#ifdef __CRATE_CPP__

//gperf stuff goes here

#endif

#endif
