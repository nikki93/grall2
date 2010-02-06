/*
 * =====================================================================================
 *
 *       Filename:  Trigger.h
 *
 *    Description:  An invisible 'trigger' object which can be used to run scripts
 *                  when an object enters an area. No actual physical simulation of
 *                  the collision takes place though, so in the real game the physical
 *                  presence of a trigger is not experienced, just it's effects are.
 *
 *        Created:  05/25/2009 06:37:01 PM
 *       Compiler:  gcc
 *
 *         Author:  Nikhilesh (nikki)
 *
 * =====================================================================================
 */

#ifndef __TRIGGER_H__
#define __TRIGGER_H__

#include "Globals.h"
#include "Objects/Main/GraLL2GameObject.h"

class Trigger :
    public GraLL2GameObject
{
    protected:
        btCollisionShape *mShape;
        bool mTouched;

    public:
        Trigger(Ogre::Vector3 pos, Ogre::Quaternion rot, NGF::ID id, NGF::PropertyList properties, Ogre::String name);
        virtual ~Trigger();

        //--- Events -------------------------------------------------------------------
        void postLoad();
        void unpausedTick(const Ogre::FrameEvent &evt);
        void pausedTick(const Ogre::FrameEvent &evt);
        NGF::MessageReply receiveMessage(NGF::Message msg);
        void collide(GameObject *other, btCollisionObject *otherPhysicsObject, btManifoldPoint &contact);

        //--- Python interface ---------------------------------------------------------
        /*
        NGF_PY_BEGIN_DECL(Trigger)
        {
        }
        NGF_PY_END_DECL
        */

        //--- Serialisation ------------------------------------------------------------
        NGF_SERIALISE_BEGIN(Trigger)
        {
            GRALL2_SERIALISE_GAMEOBJECT();
        }
        NGF_SERIALISE_END
};

#ifdef __TRIGGER_CPP__

//gperf stuff goes here

#endif

#endif
