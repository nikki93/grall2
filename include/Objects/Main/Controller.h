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

class Controller :
    public NGF::Serialisation::SerialisableGameObject,
    public NGF::Python::PythonGameObject,
    public ExtraEventListener
{
    protected:

    public:
        Controller(Ogre::Vector3 pos, Ogre::Quaternion rot, NGF::ID id, NGF::PropertyList properties, Ogre::String name);
        ~Controller();

        //--- Events -------------------------------------------------------------------
        void postLoad();
        void unpausedTick(const Ogre::FrameEvent &evt) {}
        void pausedTick(const Ogre::FrameEvent &evt) {}
        NGF::MessageReply receiveMessage(NGF::Message msg);

        //--- Serialisation ------------------------------------------------------------
        NGF_SERIALISE_BEGIN(Controller)
        {
            //Save dimension, music.
            int dimension, bonusTime;
            Ogre::String musicName;

            NGF_SERIALISE_ON_SAVE
            {
                dimension = GlbVar.dimMgr->getCurrentDimension();
                musicName = GlbVar.musicMgr->getCurrentMusic();
                bonusTime = GlbVar.bonusTime;
            }

            NGF_SERIALISE_OGRE(Int, dimension);
            NGF_SERIALISE_STRING(musicName);
            NGF_SERIALISE_OGRE(Int, bonusTime); //Save bonus time left.

            NGF_SERIALISE_ON_LOAD
            {
                GlbVar.dimMgr->setDimension(dimension);

                if (musicName != "<none>")
                    GlbVar.musicMgr->playMusic(musicName);
                else
                    GlbVar.musicMgr->stopMusic();

                GlbVar.bonusTime = bonusTime;
            }
        }
        NGF_SERIALISE_END
};

#endif

