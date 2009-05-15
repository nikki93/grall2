/*
=========================================
Utils.cpp
=========================================
*/

#include "Misc/Utils.h"

void loadLevel(Ogre::String level)
{
    GlbVar.lvlLoader->loadLevel(level);
    ExtraEventListener::callPostLoad();
}

void clearLevel()
{
    ExtraEventListener::callPreClear();
    GlbVar.goMgr->destroyAll();
}
