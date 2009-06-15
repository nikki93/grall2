/*
=========================================
ParticleEffect.cpp
=========================================
*/

#define __PARTICLEEFFECT_CPP__

#include "Objects/Misc/ParticleEffect.h"

//--- NGF events ----------------------------------------------------------------
ParticleEffect::ParticleEffect(Ogre::Vector3 pos, Ogre::Quaternion rot, NGF::ID id, NGF::PropertyList properties, Ogre::String name)
    : NGF::GameObject(pos, rot, id , properties, name),
      mTimed(false),
      mTimeLeft(0)
{
    Ogre::String ogreName = "id" + Ogre::StringConverter::toString(getID());
    addFlag("ParticleEffect");

    //Create the SceneNode.
    mNode = GlbVar.ogreSmgr->getRootSceneNode()->createChildSceneNode(ogreName, pos, rot);
    Ogre::String partTemplate = mProperties.getValue("template", 0, "ParticleFX/Explosion");
    mParticleSystem = GlbVar.ogreSmgr->createParticleSystem(ogreName, partTemplate);
    mNode->attachObject(mParticleSystem);

    //If we're timed...
    Ogre::String timeStr = mProperties.getValue("time", 0, "0");

    if (timeStr != "0")
    {
        mTimed = true;
        mTimeLeft = Ogre::StringConverter::parseReal(timeStr);
    }
}
//-------------------------------------------------------------------------------
ParticleEffect::~ParticleEffect()
{
    //Detach and destroy the ParticleEffect, and destroy the SceneNode.
    mNode->detachAllObjects();
    GlbVar.ogreSmgr->destroyParticleSystem(mParticleSystem);
    GlbVar.ogreSmgr->destroySceneNode(mNode);
}
//-------------------------------------------------------------------------------
void ParticleEffect::unpausedTick(const Ogre::FrameEvent &evt)
{
    if (mTimed)
    {
        mTimeLeft -= evt.timeSinceLastFrame;

        if (mTimeLeft <= 0)
            GlbVar.goMgr->requestDestroy(getID());
    }
}
//-------------------------------------------------------------------------------
