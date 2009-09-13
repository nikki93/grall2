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

namespace Util {

//Shows a message. Returns pointer to the MessageBox GameObject.
NGF::GameObject *showMessage(Ogre::String message, Ogre::Real time);

//Level num <-> world num.
inline unsigned int worldNumToLevelNum(unsigned int worldNum)
{
    return worldNum - GlbVar.firstLevel + 1;
}
inline unsigned int levelNumToWorldNum(unsigned int levelNum)
{
    return GlbVar.firstLevel + levelNum - 1;
}
inline Globals::Records::Record &getRecordFromLevelNum(unsigned int levelNum)
{
    return GlbVar.records.recordMap[levelNum];
}

//Serialse/deserialise.
inline void serialise(Ogre::String name)
{
    NGF::Serialisation::Serialiser::save(USER_PREFIX "Saves/" + name + ".sav");
}
inline void deserialise(Ogre::String name)
{
    NGF::Serialisation::Serialiser::load(USER_PREFIX "Saves/" + name + ".sav");
}
inline Ogre::String saveName(unsigned int worldNum)
{
    //Makes savefile name from world number of Level.
    return "Level" + Ogre::StringConverter::toString(worldNum - GlbVar.firstLevel + 1);
}

//Go to next, previous, or nth world.
inline void nextWorld()
{
    GlbVar.worldSwitch = NEXT_WORLD;
}
inline void previousWorld()
{
    GlbVar.worldSwitch = PREV_WORLD;
}
inline void gotoWorld(int n)
{
    GlbVar.worldSwitch = n;
}

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

//Clamps a comparable value.
template<class Type>
static Type clamp(Type number, Type rangeMin, Type rangeMax)
{
    Type big = (number < rangeMax) ? number : rangeMax;
    return (rangeMin > big) ? rangeMin : big;
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

//Boom!
inline void createExplosion(Ogre::Vector3 pos)
{
    GlbVar.goMgr->createObject("Light", pos, Ogre::Quaternion::IDENTITY, NGF::PropertyList::create
            ("lightType", "point")
            ("colour", "1 0.6 0")
            ("specular", "0.1 0.1 0.1")
            ("attenuation", "10 0.6 0.1 0.05")
            ("time", "1.2")
            ("fadeOutTime", "0.5")
            );

    GlbVar.goMgr->createObject("ParticleEffect", pos, Ogre::Quaternion::IDENTITY, NGF::PropertyList::create
            ("template", "ParticleFX/Explosion")
            ("time", "2")
            );
}

//Allows 'postLoad', 'preClear' etc. events.
struct ExtraEventListener
{
    //postLoad.
    virtual void postLoad() {}

    static void callPostLoad() 
    { GlbVar.goMgr->forEachGameObject(&ExtraEventListener::_callPostLoadOne); }
    static void _callPostLoadOne(NGF::GameObject *obj) 
    { ExtraEventListener *o = dynamic_cast<ExtraEventListener*>(obj); if(o) o->postLoad(); }

    //preClear.
    virtual void preClear() {}

    static void callPreClear() 
    { GlbVar.goMgr->forEachGameObject(&ExtraEventListener::_callPreClearOne); }
    static void _callPreClearOne(NGF::GameObject *obj) 
    { ExtraEventListener *o = dynamic_cast<ExtraEventListener*>(obj); if(o) o->preClear(); }
};

//Load/clear level. Calls the postLoad etc. events.
inline void loadLevel(Ogre::String level)
{
    GlbVar.lvlLoader->loadLevel(level);
    ExtraEventListener::callPostLoad();
}
inline void clearLevel()
{
    ExtraEventListener::callPreClear();
    GlbVar.goMgr->destroyAll();
}

//Makes getting a set of results from convexSweepTest easier.
struct BulletConvexResultCollector : public btDynamicsWorld::ConvexResultCallback
{
    std::set<btCollisionObject*> mHits;

    btScalar addSingleResult(btDynamicsWorld::LocalConvexResult &convexResult, bool)
    {
        mHits.insert(convexResult.m_hitCollisionObject);
        return convexResult.m_hitFraction;
    }
    
    bool needsCollision(btBroadphaseProxy* proxy0) const
    {
        return true;
    }
};

//For high-res screenshots.
void highResScreenshot(Ogre::RenderWindow* pRenderWindow, Ogre::Camera* pCamera, 
        const int& pGridSize, const Ogre::String& pFileName, const Ogre::String& pFileExtention, 
        const bool& pStitchGridImages);

} //namespace Util

#endif

