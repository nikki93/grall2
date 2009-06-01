/*
 * =====================================================================================
 *
 *       Filename:  Director.h
 *
 *    Description:  For easily specifying paths for moving objects in the editor.
 *                  When a moving object encounters this object it continues with the
 *                  direction and speed it stores.
 *
 *        Created:  06/01/2009 06:44:12 PM
 *       Compiler:  gcc
 *
 *         Author:  Nikhilesh (nikki)
 *
 * =====================================================================================
 */

#ifndef __DIRECTOR_H__
#define __DIRECTOR_H__

#ifdef __DIRECTOR_CPP__

#endif

#include "Globals.h"
#include "Objects/Main/GraLL2GameObject.h"

class Director :
    public GraLL2GameObject
{
    protected:
        btCollisionShape *mShape;
        
        Ogre::Vector3 mVelocity;
        Ogre::Quaternion mDirection;

    public:
        Director(Ogre::Vector3 pos, Ogre::Quaternion rot, NGF::ID id, NGF::PropertyList properties, Ogre::String name);
        virtual ~Director();

        //--- Events -------------------------------------------------------------------
        void postLoad();
        void unpausedTick(const Ogre::FrameEvent &evt);
        void pausedTick(const Ogre::FrameEvent &evt);
        NGF::MessageReply receiveMessage(NGF::Message msg);
        void collide(GameObject *other, btCollisionObject *otherPhysicsObject, btManifoldPoint &contact);

        //--- Python interface ---------------------------------------------------------
        /*
        NGF_PY_BEGIN_DECL(Director)
        {
        }
        NGF_PY_END_DECL
        */

        //--- Serialisation ------------------------------------------------------------
        NGF_SERIALISE_BEGIN(Director)
        {
            GRALL2_SERIALISE_GAMEOBJECT();
        }
        NGF_SERIALISE_END
};

#ifdef __Director_CPP__

//gperf stuff goes here

#endif

#endif
