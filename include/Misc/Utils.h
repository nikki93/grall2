/*
 * =====================================================================================
 *
 *       Filename:  Utils.h
 *
 *    Description:  Some utility functions and classes.
 *
 *        Created:  05/07/2009 10:03:47 PM
 *       Compiler:  gcc
 *
 *         Author:  Nikhilesh (nikki)
 *
 * =====================================================================================
 */

#ifndef __UTILS_H__
#define __UTILS_H__

#include "Globals.h"

//Tells you whether a key is down, respecting the console.
inline bool isKeyDown(OIS::KeyCode kc)
{
    return !GlbVar.console->isVisible() && GlbVar.keyboard->isKeyDown(kc);
}

//Returns the OIS mouse state, respecting the console.
inline OIS::MouseState getMouseState()
{
    if (GlbVar.console->isVisible())
        return OIS::MouseState();
    return GlbVar.mouse->getMouseState();
}

#endif
