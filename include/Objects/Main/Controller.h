/*
 * =====================================================================================
 *
 *       Filename:  Controller.h
 *
 *    Description:  Controller GameObject. He's always around, and watching over things.
 *                  ;-) He takes care of serialisation of stuff that doesn't really
 *                  belong to any object. Can also be used for non-GameObject-specific
 *                  Python scripting.
 *
 *        Created:  07/23/2009 02:36:12 PM
 *       Compiler:  gcc
 *
 *         Author:  Nikhilesh (nikki)
 *
 * =====================================================================================
 */

#ifndef __CONTROLLER_H__
#define __CONTROLLER_H__

#include "Globals.h"

//NOTE: If you're making this Python scriptable, just derive from NGF::PythonGameObject too,
//      add the 'NGF_PYTHON_DECL' and 'NGF_PYTHON_IMPL', and do the gperf stuff. Check
//      GraLL2GameObjects for guidance.

class Controller :
    public NGF::Serialisation::SerialisableGameObject
{
    protected:

    public:
        Controller(Ogre::Vector3 pos, Ogre::Quaternion rot, NGF::ID id, NGF::PropertyList properties, Ogre::String name);
        ~Controller();

        //--- Events -------------------------------------------------------------------
        void unpausedTick(const Ogre::FrameEvent &evt) {}

        void pausedTick(const Ogre::FrameEvent &evt) {}
        NGF::MessageReply receiveMessage(NGF::Message msg) { NGF_NO_REPLY(); }

        //--- Serialisation ------------------------------------------------------------
        NGF_SERIALISE_BEGIN(Controller)
        {
            //Save dimension, music.
            int dimension;
            Ogre::String musicName;

            NGF_SERIALISE_ON_SAVE
            {
                dimension = GlbVar.dimMgr->getCurrentDimension();
                musicName = GlbVar.musicMgr->getCurrentMusic();
            }

            NGF_SERIALISE_OGRE(Int, dimension);
            NGF_SERIALISE_STRING(musicName);

            NGF_SERIALISE_ON_LOAD
            {
                GlbVar.dimMgr->setDimension(dimension);

                if (musicName != "<none>")
                    GlbVar.musicMgr->playMusic(musicName);
                else
                    GlbVar.musicMgr->stopMusic();
            }
        }
        NGF_SERIALISE_END
};

#endif

