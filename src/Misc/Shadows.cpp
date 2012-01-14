/*
 * =====================================================================================
 *
 *       Filename:  Shadows.cpp
 *
 *    Description:  Shadow-related code goes here.
 *
 *        Created:  06/26/2009 03:35:00 PM
 *       Compiler:  gcc
 *
 *         Author:  Nikhilesh (nikki)
 *
 * =====================================================================================
 */

#include "Globals.h"

void initShadows()
{
    GlbVar.ogreSmgr->setShadowTechnique(Ogre::SHADOWTYPE_TEXTURE_MODULATIVE_INTEGRATED);

    //3 textures per directional light.
    GlbVar.ogreSmgr->setShadowTextureCountPerLightType(Ogre::Light::LT_DIRECTIONAL, 3);
    GlbVar.ogreSmgr->setShadowTextureCount(3);
    GlbVar.ogreSmgr->setShadowTextureConfig(0, 2048, 2048, Ogre::PF_FLOAT32_R);
    GlbVar.ogreSmgr->setShadowTextureConfig(1, 2048, 2048, Ogre::PF_FLOAT32_R);
    GlbVar.ogreSmgr->setShadowTextureConfig(2, 1024, 1024, Ogre::PF_FLOAT32_R);
    GlbVar.ogreSmgr->setShadowTextureSelfShadow(true);
    GlbVar.ogreSmgr->setShadowCasterRenderBackFaces(true);

    //Caster material.
    GlbVar.ogreSmgr->setShadowTextureCasterMaterial("BaseShadowCaster");

    //Shadow camera setup.
    Ogre::PSSMShadowCameraSetup::SplitPointList splits;
    splits.push_back(0.1f);
    splits.push_back(18.7167f);
    splits.push_back(77.4301f);
    splits.push_back(1000.0f);

    Ogre::PSSMShadowCameraSetup* pssmSetup = new Ogre::PSSMShadowCameraSetup();
    pssmSetup->setSplitPoints(splits);
    pssmSetup->setSplitPadding(10);
    pssmSetup->setOptimalAdjustFactor(0, 5);
    pssmSetup->setOptimalAdjustFactor(1, 3);
    pssmSetup->setOptimalAdjustFactor(2, 0.1);
    GlbVar.ogreSmgr->setShadowCameraSetup(Ogre::ShadowCameraSetupPtr(pssmSetup));
}

