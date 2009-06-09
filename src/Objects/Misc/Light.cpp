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
      mTime(0)
{
    Ogre::String ogreName = "id" + Ogre::StringConverter::toString(getID());
    addFlag("Light");

    //Create the SceneNode.
    mNode = GlbVar.ogreSmgr->getRootSceneNode()->createChildSceneNode(ogreName, pos, rot);

    //Get light type.
    Ogre::String type = mProperties.getValue("lightType", 0, "point");

    //Create the Light.
    mLight = GlbVar.ogreSmgr->createLight(ogreName);

    mLight->setType((type == "spot") ? Ogre::Light::LT_SPOTLIGHT : Ogre::Light::LT_POINT);

    mLight->setSpecularColour(Ogre::ColourValue(0.2,0.2,0.2));

    //Set light properties.
    mLight->setDiffuseColour(Ogre::ColourValue(
            Ogre::StringConverter::parseReal(mProperties.getValue("colour", 0, "1")),
            Ogre::StringConverter::parseReal(mProperties.getValue("colour", 1, "1")),
            Ogre::StringConverter::parseReal(mProperties.getValue("colour", 2, "1"))
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
    }
    else
    {
        mLight->setCastShadows(false);
    }


    //Attach it to the SceneNode.
    mNode->attachObject(mLight);
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
