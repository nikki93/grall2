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

/*
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
};
*/

void initShadows()
{
    const unsigned int numShadowTex = 4;

    GlbVar.ogreSmgr->setShadowTextureSelfShadow(true);
    GlbVar.ogreSmgr->setShadowTextureCasterMaterial("shadow_caster");

    GlbVar.ogreSmgr->setShadowTextureCount(numShadowTex);
    GlbVar.ogreSmgr->setShadowTextureSize(256);
    GlbVar.ogreSmgr->setShadowTexturePixelFormat(Ogre::PF_FLOAT16_RGB);
    GlbVar.ogreSmgr->setShadowTextureCountPerLightType(Ogre::Light::LT_SPOTLIGHT, numShadowTex);
    GlbVar.ogreSmgr->setShadowCasterRenderBackFaces(false);

    for (unsigned i = 0; i < numShadowTex; ++i) 
    {
        Ogre::TexturePtr tex = GlbVar.ogreSmgr->getShadowTexture(i);
        Ogre::Viewport *vp = tex->getBuffer()->getRenderTarget()->getViewport(0);
        vp->setBackgroundColour(Ogre::ColourValue(1,1,1,1));
        vp->setClearEveryFrame(true);
    }
    GlbVar.ogreSmgr->setShadowTechnique(Ogre::SHADOWTYPE_TEXTURE_ADDITIVE_INTEGRATED);

    //Debug overlays.
    int w = GlbVar.ogreWindow->getWidth();
    for (unsigned i = 0; i < numShadowTex; ++i)
    {
        MyGUI::StaticImagePtr img = GlbVar.gui->createWidget<MyGUI::StaticImage>("StaticImage", w - 250 , 50 + 210*i, 200, 200, MyGUI::Align::Default, "Main");
        img->setImageTexture(GlbVar.ogreSmgr->getShadowTexture(i)->getName());
        img->setVisible(true);
    }
}

