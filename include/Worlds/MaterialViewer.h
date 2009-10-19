/*
 * =====================================================================================
 *
 *       Filename:  MaterialViewer.h
 *
 *    Description:  The material viewer.
 *
 *        Created:  09/29/2009 01:53:03 PM
 *       Compiler:  gcc
 *
 *         Author:  Nikhilesh S (nikki)
 *
 * =====================================================================================
 */

#ifndef __MATERIALVIEWER_H__
#define __MATERIALVIEWER_H__

#include "Globals.h"
#include "OgreScriptCompiler.h"

class MaterialViewer : 
    public NGF::World
{
    protected:
        Ogre::Entity *mEntity;
        Ogre::SceneNode *mNode;

        Ogre::SceneNode *mCameraYawNode;
        Ogre::SceneNode *mCameraPitchNode;
        Ogre::Real mCameraDist;

        typedef std::map<Ogre::String, Ogre::String> MaterialSettings;

        Ogre::String mMaterialType;
        MaterialSettings mMaterialSettings;

        int mMaterialCount;

        Ogre::ScriptCompiler *mCompiler;

        MyGUI::WindowPtr mWindow;

    public:
        MaterialViewer() 
            : mMaterialCount(0),
              mCompiler(new Ogre::ScriptCompiler())
        {
        }

        ~MaterialViewer() 
        {
            delete mCompiler;
        }

        void init();
        void tick(const Ogre::FrameEvent &evt);
        void stop();

        Ogre::String createMaterialSource(const Ogre::String& materialName);
        void compileMaterial();

        void onClickApply(MyGUI::WidgetPtr);
        void onClickSave(MyGUI::WidgetPtr);
        void onClickCreateLight(MyGUI::WidgetPtr);
};

#endif
