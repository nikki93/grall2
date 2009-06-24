/*
 * =====================================================================================
 *
 *       Filename:  Checkpoint.h
 *
 *    Description:  Mid-level save point.
 *
 *        Created:  06/24/2009 09:39:13 PM
 *       Compiler:  gcc
 *
 *         Author:  Nikhilesh (nikki)
 *
 * =====================================================================================
 */

#ifndef __CHECKPOINT_H__
#define __CHECKPOINT_H__

#include "Globals.h"
#include "Objects/Main/GraLL2GameObject.h"

class Checkpoint :
    public GraLL2GameObject
{
    protected:
        btCollisionShape *mShape;
        Ogre::Entity *mEntity;
        
        bool mEnabled;

    public:
        Checkpoint(Ogre::Vector3 pos, Ogre::Quaternion rot, NGF::ID id, NGF::PropertyList properties, Ogre::String name);
        virtual ~Checkpoint();

        //--- Events -------------------------------------------------------------------
        void postLoad();
        void unpausedTick(const Ogre::FrameEvent &evt);
        void pausedTick(const Ogre::FrameEvent &evt);
        NGF::MessageReply receiveMessage(NGF::Message msg);
        void collide(GameObject *other, btCollisionObject *otherPhysicsObject, btManifoldPoint &contact);

        //--- Python interface ---------------------------------------------------------
        /*
        NGF_PY_BEGIN_DECL(Checkpoint)
        {
        }
        NGF_PY_END_DECL
        */

        //--- Serialisation ------------------------------------------------------------
        NGF_SERIALISE_BEGIN(Checkpoint)
        {
            GRALL2_SERIALISE_GAMEOBJECT();

            NGF_SERIALISE_OGRE(Bool, mEnabled);

            NGF_SERIALISE_ON_LOAD
            {
                if (!mEnabled)
                    mEntity->setMaterialName("Objects/Player");
            }
        }
        NGF_SERIALISE_END
};

#ifdef __CHECKPOINT_CPP__

//gperf stuff goes here.

#endif

#endif
