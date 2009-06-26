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
    GlbVar.ogreSmgr->setShadowTextureSelfShadow(true);
    GlbVar.ogreSmgr->setShadowTextureCasterMaterial("shadow_caster");

    GlbVar.ogreSmgr->setShadowTextureCount(4);
    GlbVar.ogreSmgr->setShadowTextureSize(1024);
    GlbVar.ogreSmgr->setShadowTexturePixelFormat(Ogre::PF_FLOAT16_RGB);

    GlbVar.ogreSmgr->setShadowCasterRenderBackFaces(false);

    const unsigned numShadowRTTs = GlbVar.ogreSmgr->getShadowTextureCount();
    for (unsigned i = 0; i < numShadowRTTs; ++i) {
        Ogre::TexturePtr tex = GlbVar.ogreSmgr->getShadowTexture(i);
        Ogre::Viewport *vp = tex->getBuffer()->getRenderTarget()->getViewport(0);
        vp->setBackgroundColour(Ogre::ColourValue(0.2, 0.2, 0.2, 0.2));
        vp->setClearEveryFrame(true);
    }

    GlbVar.ogreSmgr->setShadowTechnique(Ogre::SHADOWTYPE_TEXTURE_ADDITIVE_INTEGRATED);
}

