/*
 * =====================================================================================
 *
 *       Filename:  TestWorld.h
 *
 *    Description:  Has the World and GameObject 'lists', for registration.
 *
 *        Created:  04/28/2009 03:59:06 PM
 *       Compiler:  gcc
 *
 *         Author:  Nikhilesh (nikki)
 *
 * =====================================================================================
 */

#include "Globals.h"

//GameObject headers.
#include "Objects/Main/GraLL2GameObject.h"
#include "Objects/Main/Player.h"

//World headers.
#include "Worlds/TestWorld.h"

//Register GameObject types.
void registerGameObjectTypes()
{
    NGF_REGISTER_OBJECT_TYPE(Player);
}

//Register worlds.
void addWorlds()
{
    GlbVar.woMgr->addWorld(new TestWorld());
}
