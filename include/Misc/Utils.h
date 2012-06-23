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
#include "Misc/Utils.h"
#include "Worlds/Level.h"

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/fstream.hpp>

namespace Util {

//Checks whether a file or directory exists.
inline bool pathExists(const Ogre::String &filename)
{
    return boost::filesystem::exists(filename);
}
inline void touchFile(const Ogre::String &filename)
{
    std::ofstream out(filename.c_str());
    out << std::endl;
    out.close();
}
inline void touchDirectory(const Ogre::String &path)
{
    boost::filesystem::create_directory(path);
}
inline void requireDirectory(const Ogre::String &path)
{
    if (!pathExists(path))
        touchDirectory(path);
}
inline void requireFile(const Ogre::String &path)
{
    if (!pathExists(path))
        touchFile(path);
}

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
Ogre::String checkpointName(unsigned int worldNum);
inline void saveCheckpoint()
{
    Util::serialise(Util::checkpointName(GlbVar.woMgr->getCurrentWorldIndex()));
    Util::getRecordFromLevelNum(Util::worldNumToLevelNum(GlbVar.woMgr->getCurrentWorldIndex())).checkpoint = true;
}
inline bool loadCheckpoint()
{
    if (Util::getRecordFromLevelNum(Util::worldNumToLevelNum(GlbVar.woMgr->getCurrentWorldIndex())).checkpoint)
    {
        Util::deserialise(Util::checkpointName(GlbVar.woMgr->getCurrentWorldIndex()));
        return true;
    }
    return false;
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

//Screenshots.
void screenshot(Ogre::String filename, const Ogre::String &extension);
void highResScreenshot(Ogre::RenderWindow* pRenderWindow, Ogre::Camera* pCamera, 
        const int& pGridSize, Ogre::String pFileName, const Ogre::String& pFileExtention, 
        const bool& pStitchGridImages);

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

//Adds a directory recursively, without path in resource names. Also adds .zip archives.
inline void addResourceLocationRecursive(Ogre::String path, Ogre::String group)
{
    Ogre::ResourceGroupManager::getSingleton().addResourceLocation(path, "FileSystem", "General");

    Ogre::Archive *arch = Ogre::ArchiveManager::getSingleton().load(path, "FileSystem");
    Ogre::StringVectorPtr files = arch->find("*", true, true);
    for (Ogre::StringVector::iterator iter = files->begin(); iter != files->end(); ++iter)
    {
        Ogre::String resource = path + "/" + *iter;
        LOG("Adding resource directory: " + resource);
        Ogre::ResourceGroupManager::getSingleton().addResourceLocation(resource, "FileSystem", "General");
    }

    files = arch->find("*.zip", true, false);
    for (Ogre::StringVector::iterator iter = files->begin(); iter != files->end(); ++iter)
    {
        Ogre::String resource = path + "/" + *iter;
        LOG("Adding .zip archive: " + resource);
        Ogre::ResourceGroupManager::getSingleton().addResourceLocation(resource, "Zip", "General");
    }
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
        matName = currSub->getSubMesh()->getMaterialName();

        size_t pos = matName.find_last_of('/');
        if (pos != std::string::npos)
        {
            matName = "Brushes" + matName.substr(pos);
            currSub->setMaterialName(matName);
        }
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
inline void loadNgf(Ogre::String level)
{
    GlbVar.lvlLoader->loadLevel(level);
    ExtraEventListener::callPostLoad();
}
inline void clearLevel()
{
    ExtraEventListener::callPreClear();
    GlbVar.goMgr->destroyAll();
}
inline void loadUserLevel(const Ogre::String &ngfName)
{
    unsigned int worldNum = GlbVar.firstLevel - 1;
    Level *userLevel = dynamic_cast<Level*>(GlbVar.woMgr->getWorld(worldNum));
    userLevel->setNgfName(ngfName);
    userLevel->setCaption(ngfName);

    GlbVar.woMgr->gotoWorld(worldNum);
}

//Preload a material.
inline void preloadMaterial(const Ogre::String &name)
{
    GlbVar.preloadEntity->setMaterialName(name);
}

//Reload all materials.
void reloadMaterials();

//Write Cg shader config .h file.
inline void writeShaderConfig()
{
    std::ofstream config((USER_PREFIX "Content/ShaderConfig.h").c_str());

    config << "#define SET_NORMAL_MAPPING " << GlbVar.settings.graphics.normalMapping << std::endl;
    config << "#define SET_PARALLAX_MAPPING " << GlbVar.settings.graphics.parallaxMapping << std::endl;
    config << "#define SET_SHADOWS " << GlbVar.settings.graphics.shadows << std::endl;
}

//Create the sun.
inline void reloadSun()
{
    if (GlbVar.sun)
        GlbVar.ogreSmgr->destroyLight(GlbVar.sun);
    GlbVar.sun = GlbVar.ogreSmgr->createLight("sun");
    GlbVar.sun->setType(Ogre::Light::LT_DIRECTIONAL);
    GlbVar.sun->setDiffuseColour(Ogre::ColourValue(0.22,0.22,0.22));
    GlbVar.sun->setSpecularColour(Ogre::ColourValue(0,0,0));
    GlbVar.sun->setDirection(Ogre::Vector3(1,-2.5,1)); 
    GlbVar.sun->setCastShadows(GlbVar.settings.graphics.shadows);
}

} //namespace Util

#endif

