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

struct ShadowListener : public Ogre::SceneManager::Listener
{
    void shadowTextureCasterPreViewProj(Ogre::Light *light, Ogre::Camera *cam, size_t)
    {
        cam->setNearClipDistance(0.01);
        cam->setFarClipDistance(light->getAttenuationRange());
    }

    //Pure virtuals.
    void shadowTexturesUpdated(size_t) {}
    void shadowTextureReceiverPreViewProj(Ogre::Light*, Ogre::Frustum*) {}
    void preFindVisibleObjects(Ogre::SceneManager*, Ogre::SceneManager::IlluminationRenderStage, Ogre::Viewport*) {}
    void postFindVisibleObjects(Ogre::SceneManager*, Ogre::SceneManager::IlluminationRenderStage, Ogre::Viewport*) {}
} shadowListener;

void initShadows()
{
    const int numShadowTex = 1;

    GlbVar.ogreSmgr->setShadowTextureSelfShadow(true);
    GlbVar.ogreSmgr->setShadowTextureCasterMaterial("BaseShadowCaster");
    GlbVar.ogreSmgr->setShadowTexturePixelFormat(Ogre::PF_FLOAT32_RGB);
    GlbVar.ogreSmgr->setShadowTextureSize(2048);
    GlbVar.ogreSmgr->setShadowCasterRenderBackFaces(false);
    GlbVar.ogreSmgr->setShadowTechnique(Ogre::SHADOWTYPE_TEXTURE_ADDITIVE_INTEGRATED);

    //Debug overlays
    unsigned int w = GlbVar.ogreWindow->getWidth();
    for (unsigned int i = 0; i < numShadowTex; ++i)
    {
        MyGUI::StaticImagePtr img = GlbVar.gui->createWidget<MyGUI::StaticImage>("StaticImage", w - 250 , 50 + 210*i, 200, 200, MyGUI::Align::Default, "Main");
        img->setImageTexture(GlbVar.ogreSmgr->getShadowTexture(i)->getName());
        img->setVisible(true);
    }
}

