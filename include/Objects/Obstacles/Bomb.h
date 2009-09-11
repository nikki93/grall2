/*
 * =====================================================================================
 *
 *       Filename:  Bomb.h
 *
 *    Description:  A stationary Bomb. Don't touch! :P
 *
 *        Created:  06/14/2009 04:02:56 PM
 *       Compiler:  gcc
 *
 *         Author:  Nikhilesh (nikki)
 *
 * =====================================================================================
 */

#ifndef __BOMB_H__
#define __BOMB_H__

#include "Globals.h"
#include "Objects/Main/GraLL2GameObject.h"

class Bomb :
    public GraLL2GameObject
{
    protected:
        btCollisionShape *mShape;
        Ogre::Entity *mEntity;
        
        bool mGreen; //Green bombs are crate-fearing.
        bool mExploded;

    public:
        Bomb(Ogre::Vector3 pos, Ogre::Quaternion rot, NGF::ID id, NGF::PropertyList properties, Ogre::String name);
        virtual ~Bomb();

        //--- Events -------------------------------------------------------------------
        void postLoad();
        void unpausedTick(const Ogre::FrameEvent &evt);
        void pausedTick(const Ogre::FrameEvent &evt);
        NGF::MessageReply receiveMessage(NGF::Message msg);
        void collide(GameObject *other, btCollisionObject *otherPhysicsObject, btManifoldPoint &contact);

        //--- Non-NGF ------------------------------------------------------------------
        void explode();

        //--- Python interface ---------------------------------------------------------
        /*
        NGF_PY_BEGIN_DECL(Bomb)
        {
        }
        NGF_PY_END_DECL
        */

        //--- Serialisation ------------------------------------------------------------
        NGF_SERIALISE_BEGIN(Bomb)
        {
            GRALL2_SERIALISE_GAMEOBJECT();

            NGF_SERIALISE_OGRE(Bool, mExploded);
        }
        NGF_SERIALISE_END
};

#ifdef __BOMB_CPP__

//gperf stuff goes here.

#endif

#endif
