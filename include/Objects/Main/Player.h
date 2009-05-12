/*
 * =====================================================================================
 *
 *       Filename:  Player.h
 *
 *    Description:  The Player GameObject, which specifies the behaviour of the Player.
 *
 *        Created:  05/03/2009 03:04:54 PM
 *       Compiler:  gcc
 *
 *         Author:  Nikhilesh (nikki)
 *
 * =====================================================================================
 */

#ifndef __PLAYER_H__
#define __PLAYER_H__

#include "Globals.h"
#include "Objects/Main/GraLL2GameObject.h"

class Player :
    public GraLL2GameObject
{
    protected:
        Ogre::SceneNode *mControlNode;
        Ogre::Entity *mEntity;
        btCollisionShape *mShape;
        NGF::GameObject *mCameraHandler;

    public:
        Player(Ogre::Vector3 pos, Ogre::Quaternion rot, NGF::ID id, NGF::PropertyList properties, Ogre::String name);
        virtual ~Player();

        //--- Events -------------------------------------------------------------------
        void unpausedTick(const Ogre::FrameEvent &evt);
        void pausedTick(const Ogre::FrameEvent &evt);
        NGF::MessageReply receiveMessage(NGF::Message msg);
        void collide(GameObject *other, btCollisionObject *otherPhysicsObject, btManifoldPoint &contact);

        //--- Python interface ---------------------------------------------------------
        /*
        NGF_PY_BEGIN_DECL(Player)
        {
        }
        NGF_PY_END_DECL
        */

        //--- Serialisation ------------------------------------------------------------
        NGF_SERIALISE_BEGIN(Player)
        {
            Ogre::Quaternion controlRot;

            NGF_SERIALISE_ON_SAVE
            {
                controlRot = mControlNode->getOrientation();
            }

            GRALL2_SERIALISE_GAMEOBJECT();
            NGF_SERIALISE_OGRE(Quaternion, controlRot);

            NGF_SERIALISE_ON_LOAD
            {
                mControlNode->setOrientation(controlRot);
            }
        }
        NGF_SERIALISE_END
};

#ifdef __PLAYER_CPP__

//gperf stuff goes here

#endif

#endif
