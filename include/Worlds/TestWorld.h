/*
 * =====================================================================================
 *
 *       Filename:  TestWorld.h
 *
 *    Description:  An NGF::World for pure testing purposes, and will not be added to
 *                  the Worlds-list on release.
 *
 *        Created:  04/28/2009 03:51:45 PM
 *       Compiler:  gcc
 *
 *         Author:  Nikhilesh (nikki)
 *
 * =====================================================================================
 */

#ifndef _TESTWORLD_H_
#define _TESTWORLD_H_

#include "Globals.h"

class TestWorld : 
    public NGF::World
{
    protected:
        btRigidBody *mTestBody;
        btCollisionShape *mTestShape;

    public:
            TestWorld() {}
            ~TestWorld() {}

            void init();
            void tick(const Ogre::FrameEvent &evt);
            void stop();
};

#endif
