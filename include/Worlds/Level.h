/*
 * =====================================================================================
 *
 *       Filename:  Level.h
 *
 *    Description:  Usual level, does usual stuff. Handles loading of level file 
 *                  restoration of checkpoint state, music etc.
 *
 *        Created:  06/23/2009 03:09:46 PM
 *       Compiler:  gcc
 *
 *         Author:  Nikhilesh (nikki)
 *
 * =====================================================================================
 */

#ifndef __LEVEL_H__
#define __LEVEL_H__

#include "Globals.h"

class Level : 
    public NGF::World
{
    protected:
        Ogre::String mLevelName;

    public:
        Level(Ogre::String levelName);
        ~Level() {}

        void init();
        void tick(const Ogre::FrameEvent &evt);
        void stop();
};

#endif
