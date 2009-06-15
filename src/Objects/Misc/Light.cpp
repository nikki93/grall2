/*
=========================================
Light.cpp
=========================================
*/

#define __LIGHT_CPP__

#include "Objects/Misc/Light.h"

//--- NGF events ----------------------------------------------------------------
Light::Light(Ogre::Vector3 pos, Ogre::Quaternion rot, NGF::ID id, NGF::PropertyList properties, Ogre::String name)
    : NGF::GameObject(pos, rot, id , properties, name),
      mTimed(false),
      mTimeLeft(0)
{
    Ogre::String ogreName = "id" + Ogre::StringConverter::toString(getID());
    addFlag("Light");

    //Create the SceneNode and Light.
    mNode = GlbVar.ogreSmgr->getRootSceneNode()->createChildSceneNode(ogreName, pos, rot);
    mLight = GlbVar.ogreSmgr->createLight(ogreName);
    mNode->attachObject(mLight);

    //Set light properties.
    Ogre::String type = mProperties.getValue("lightType", 0, "point");
    mLight->setType((type == "spot") ? Ogre::Light::LT_SPOTLIGHT : Ogre::Light::LT_POINT);

    mLight->setDiffuseColour(mDiffuseColour = Ogre::ColourValue(
            Ogre::StringConverter::parseReal(mProperties.getValue("colour", 0, "1")),
            Ogre::StringConverter::parseReal(mProperties.getValue("colour", 1, "1")),
            Ogre::StringConverter::parseReal(mProperties.getValue("colour", 2, "1"))
            ));

    mLight->setSpecularColour(mSpecularColour = Ogre::ColourValue(
            Ogre::StringConverter::parseReal(mProperties.getValue("specular", 0, "0.4")),
            Ogre::StringConverter::parseReal(mProperties.getValue("specular", 1, "0.4")),
            Ogre::StringConverter::parseReal(mProperties.getValue("specular", 2, "0.4"))
            ));

    mLight->setAttenuation(
            Ogre::StringConverter::parseReal(mProperties.getValue("attenuation", 0, "10")),
            Ogre::StringConverter::parseReal(mProperties.getValue("attenuation", 1, "0")),
            Ogre::StringConverter::parseReal(mProperties.getValue("attenuation", 2, "0")),
            Ogre::StringConverter::parseReal(mProperties.getValue("attenuation", 3, "0.1"))
            );

    if (mLight->getType() == Ogre::Light::LT_SPOTLIGHT)
    {
        mLight->setSpotlightRange(
                Ogre::Degree(Ogre::StringConverter::parseReal(mProperties.getValue("innerAngle", 0, "0"))),
                Ogre::Degree(Ogre::StringConverter::parseReal(mProperties.getValue("outerAngle", 1, "45")))
                );
        mLight->setDirection(Ogre::Vector3(0,0,-1));
        mLight->setCastShadows(true);
    }
    else
    {
        mLight->setCastShadows(false); //Non-spotlights don't cast shadows.
    }

    //If we're timed...
    Ogre::String timeStr = mProperties.getValue("time", 0, "0");

    if (timeStr != "0")
    {
        mFadeOutTime = Ogre::StringConverter::parseReal(mProperties.getValue("fadeOutTime", 0, "0.5"));
        mTimed = true;
        mTimeLeft = Ogre::StringConverter::parseReal(timeStr);
    }
}
//-------------------------------------------------------------------------------
Light::~Light()
{
    //Detach and destroy the Light, and destroy the SceneNode.
    mNode->detachAllObjects();
    GlbVar.ogreSmgr->destroyLight(mLight);
    GlbVar.ogreSmgr->destroySceneNode(mNode);
}
//-------------------------------------------------------------------------------
void Light::unpausedTick(const Ogre::FrameEvent &evt)
{
    if (mTimed)
    {
        mTimeLeft -= evt.timeSinceLastFrame;

        //Fade out.
        if (mTimeLeft < mFadeOutTime)
        {
            mLight->setDiffuseColour(mDiffuseColour * clamp(mTimeLeft * 2, 0.0f, 1.0f));
            mLight->setSpecularColour(mSpecularColour * clamp(mTimeLeft * 2, 0.0f, 1.0f));
        }

        if (mTimeLeft <= 0)
            GlbVar.goMgr->requestDestroy(getID());
    }
}
//-------------------------------------------------------------------------------
