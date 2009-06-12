/*
 * =====================================================================================
 *
 *       Filename:  MessageBox.h
 *
 *    Description:  Displays a message at the bottom of the screen for a while.
 *
 *        Created:  06/12/2009 12:38:13 PM
 *       Compiler:  gcc
 *
 *         Author:  Nikhilesh (nikki)
 *
 * =====================================================================================
 */

#ifndef __MESSAGEBOX_H__
#define __MESSAGEBOX_H__

#include "Globals.h"

//Not really Python scripted, but Python-enabled to allow control of MessageBox objects
//(creation/destruction).

class MessageBox :
    public NGF::Python::PythonGameObject,
    public NGF::Serialisation::SerialisableGameObject
{
    protected:
        Ogre::String mMessageStr;
        MyGUI::EditPtr mMessage;

        bool mTimed;
        Ogre::Real mTimeLeft;

    public:
        MessageBox(Ogre::Vector3 pos, Ogre::Quaternion rot, NGF::ID id, NGF::PropertyList properties, Ogre::String name);
        virtual ~MessageBox();

        //--- Events -------------------------------------------------------------------
        void unpausedTick(const Ogre::FrameEvent &evt);

        void pausedTick(const Ogre::FrameEvent &evt) {}
        NGF::MessageReply receiveMessage(NGF::Message msg) { NGF_NO_REPLY(); }

        //--- Serialisation ------------------------------------------------------------
        NGF_SERIALISE_BEGIN(MessageBox)
        {
            NGF_SERIALISE_OGRE(Real, mTimeLeft);
        }
        NGF_SERIALISE_END
};

#endif
