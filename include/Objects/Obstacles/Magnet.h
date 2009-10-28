/*
 * =====================================================================================
 *
 *       Filename:  Magnet.h
 *
 *    Description:  Attracts or repels 'magnetic' objects (Player, magnetic RigidBody
 *                  etc.).
 *
 *        Created:  10/28/2009 06:50:31 PM
 *       Compiler:  gcc
 *
 *         Author:  Nikhilesh S (nikki)
 *
 * =====================================================================================
 */

#ifndef __MAGNET_H__
#define __MAGNET_H__

#include "Globals.h"
#include "Objects/Main/GraLL2GameObject.h"

#include "BulletCollision/CollisionDispatch/btGhostObject.h"

class Magnet :
    public GraLL2GameObject
{
    protected:
        btCollisionShape *mShape;
        Ogre::Entity *mEntity;

        btCollisionShape *mFieldShape;
        btPairCachingGhostObject *mFieldObject;

        Ogre::Real mForce;
        Ogre::Real mRadius;
        bool mEnabled;

    public:
        Magnet(Ogre::Vector3 pos, Ogre::Quaternion rot, NGF::ID id, NGF::PropertyList properties, Ogre::String name);
        virtual ~Magnet();

        //--- Events -------------------------------------------------------------------
        void postLoad();
        void unpausedTick(const Ogre::FrameEvent &evt);
        void pausedTick(const Ogre::FrameEvent &evt);
        NGF::MessageReply receiveMessage(NGF::Message msg);
        void collide(GameObject *other, btCollisionObject *otherPhysicsObject, btManifoldPoint &contact);

        //--- Non-NGF ------------------------------------------------------------------
        inline void updateFieldPos() 
        { 
            btTransform trans; 
            mBody->getMotionState()->getWorldTransform(trans); 
            mFieldObject->setWorldTransform(trans); 
        }

        //--- Python interface ---------------------------------------------------------
        /*
        NGF_PY_BEGIN_DECL(Magnet)
        {
        }
        NGF_PY_END_DECL
        */

        //--- Serialisation ------------------------------------------------------------
        NGF_SERIALISE_BEGIN(Magnet)
        {
            GRALL2_SERIALISE_GAMEOBJECT();
        }
        NGF_SERIALISE_END
};

#ifdef __MAGNET_CPP__

//gperf stuff goes here.

#endif

#endif
