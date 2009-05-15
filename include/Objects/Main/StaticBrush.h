/*
 * =====================================================================================
 *
 *       Filename:  StaticBrush.h
 *
 *    Description:  A static 'brush' (GameObject whose mesh you can edit in Blender for
 *                  each instance). Used for level geometry, props, etc. Dimensional,
 *                  so cool things are possible. :-)
 *
 *        Created:  05/04/2009 07:08:42 PM
 *       Compiler:  gcc
 *
 *         Author:  Nikhilesh (nikki)
 *
 * =====================================================================================
 */

#ifndef __STATICBRUSH_H__
#define __STATICBRUSH_H__

#include "Globals.h"
#include "Objects/Main/GraLL2GameObject.h"

class StaticBrush :
    public GraLL2GameObject
{
    protected:
        Ogre::Entity *mEntity;
        btCollisionShape *mShape;

    public:
        StaticBrush(Ogre::Vector3 pos, Ogre::Quaternion rot, NGF::ID id, NGF::PropertyList properties, Ogre::String name);
        virtual ~StaticBrush();

        //--- Events -------------------------------------------------------------------
        void postLoad();
        void unpausedTick(const Ogre::FrameEvent &evt);
        void pausedTick(const Ogre::FrameEvent &evt);
        NGF::MessageReply receiveMessage(NGF::Message msg);
        void collide(GameObject *other, btCollisionObject *otherPhysicsObject, btManifoldPoint &contact);

        //--- Python interface ---------------------------------------------------------
        /*
        NGF_PY_BEGIN_DECL(StaticBrush)
        {
        }
        NGF_PY_END_DECL
        */

        //--- Serialisation ------------------------------------------------------------
        NGF_SERIALISE_BEGIN(StaticBrush)
        {
            GRALL2_SERIALISE_GAMEOBJECT();
        }
        NGF_SERIALISE_END
};

#ifdef __STATICBRUSH_CPP__

//gperf stuff goes here

#endif

#endif
