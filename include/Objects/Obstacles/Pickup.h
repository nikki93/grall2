/*
 * =====================================================================================
 *
 *       Filename:  Pickup.h
 *
 *    Description:  A generic 'Pickup' object. Can be used for keys, powerups, whatever.
 *
 *        Created:  09/13/2009 03:28:28 PM
 *       Compiler:  gcc
 *
 *         Author:  Nikhilesh (nikki)
 *
 * =====================================================================================
 */

#ifndef __PICKUP_H__
#define __PICKUP_H__

#include "Globals.h"
#include "Objects/Main/GraLL2GameObject.h"

class Pickup :
    public GraLL2GameObject
{
    protected:
        btCollisionShape *mShape;
        Ogre::Entity *mEntity;

        Ogre::String mPickupType; //The pickup type (KeyR, KeyG etc.). Maps to the pickups map key in Player.

        Ogre::Real mSpin;
        Ogre::Real mBob;
        Ogre::Real mTime;

    public:
        Pickup(Ogre::Vector3 pos, Ogre::Quaternion rot, NGF::ID id, NGF::PropertyList properties, Ogre::String name);
        virtual ~Pickup();

        //--- Events -------------------------------------------------------------------
        void postLoad();
        void unpausedTick(const Ogre::FrameEvent &evt);
        void pausedTick(const Ogre::FrameEvent &evt);
        NGF::MessageReply receiveMessage(NGF::Message msg);
        void collide(GameObject *other, btCollisionObject *otherPhysicsObject, btManifoldPoint &contact);

        //--- Python interface ---------------------------------------------------------
        /*
        NGF_PY_BEGIN_DECL(Pickup)
        {
        }
        NGF_PY_END_DECL
        */

        //--- Serialisation ------------------------------------------------------------
        NGF_SERIALISE_BEGIN(Pickup)
        {
            GRALL2_SERIALISE_GAMEOBJECT();

            NGF_SERIALISE_STRING(mPickupType);
        }
        NGF_SERIALISE_END
};

#ifdef __PICKUP_CPP__

//gperf stuff goes here.

#endif

#endif
