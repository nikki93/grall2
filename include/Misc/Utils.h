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

//Fixes the Brush materials, ie., makes the Brush use the materials defined manually in
//data/Brushes.material.
//
//Blender exported materials using TEXFACE end with the texture filename, so this is
//easy.
inline void fixBrushMaterials(Ogre::Entity *ent)
{
    Ogre::String matName;
    Ogre::SubEntity *currSub;

    int numSubs = ent->getNumSubEntities();
    for (int i = 0; i < numSubs; ++i)
    {
        currSub = ent->getSubEntity(i);
        matName = currSub->getMaterialName();
        matName = "Brushes" + matName.substr(matName.find_last_of('/'));
        currSub->setMaterialName(matName);
    }
}

#endif
