/*
 * =====================================================================================
 *
 *       Filename:  GravityManager.h
 *
 *    Description:  Manages up axis and gravity.
 *
 *        Created:  11/09/2009 07:25:54 AM
 *       Compiler:  gcc
 *
 *         Author:  Nikhilesh S (nikki)
 *
 * =====================================================================================
 */

#ifndef __GRAVITYMANAGER_H__
#define __GRAVITYMANAGER_H__

#include "Globals.h"

#define DOWN_GRAVITY btVector3(0,-10,0)
#define UP_GRAVITY btVector3(0,10,0)

class GravityManager
{
    protected:
        bool mUp;

        inline void updateGravity()
        {
            //When up, gravity is down.
            GlbVar.phyWorld->setGravity(mUp ? DOWN_GRAVITY : UP_GRAVITY);
        }

    public:
        GravityManager()
            : mUp(true)
        {
            GlbVar.phyWorld->setGravity(DOWN_GRAVITY);
        }

        inline void invert()
        {
            mUp = !mUp;
            updateGravity();
            GlbVar.ogreCamera->roll(Ogre::Degree(180));
        }

        inline void setUp(bool up)
        {
            mUp = up;
            updateGravity();
        }

        inline Ogre::Vector3 getUpVector()
        {
            return mUp ? Ogre::Vector3::UNIT_Y : Ogre::Vector3::NEGATIVE_UNIT_Y;
        }

        inline bool isUp()
        {
            return mUp;
        }

        inline Ogre::Real getSign()
        {
            return mUp ? 1 : -1;
        }
};

#endif
