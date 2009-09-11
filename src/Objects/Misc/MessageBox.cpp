/*
=========================================
MessageBox.cpp
=========================================
*/

#define __MessageBox_CPP__

#include "Objects/Misc/MessageBox.h"

#define FONT_HEIGHT 17.5
#define FONT_WIDTH 8
#define MAX_ALPHA 0.8

//--- NGF events ----------------------------------------------------------------
MessageBox::MessageBox(Ogre::Vector3 pos, Ogre::Quaternion rot, NGF::ID id, NGF::PropertyList properties, Ogre::String name)
    : NGF::GameObject(pos, rot, id , properties, name),
      mTimed(true)
{
    Ogre::String ogreName = "id" + Ogre::StringConverter::toString(getID());
    addFlag("MessageBox");

    //Kill the old fellow.
    if (GlbVar.currMessageBox)
        GlbVar.goMgr->destroyObject(GlbVar.currMessageBox->getID());
    GlbVar.currMessageBox = this;

    //Extract properties.
    mTimeLeft = Ogre::StringConverter::parseReal(mProperties.getValue("time", 0, "5"));
    mMessageStr = mProperties.getValue("message", 0, "<no message>");

    //We might be not-timed.
    if (mTimeLeft == 0)
        mTimed = false;

    //Position it.
    int winHeight = GlbVar.ogreWindow->getHeight();
    int winWidth = GlbVar.ogreWindow->getWidth();

    int lines = 1;
    int maxWidth = 0;
    int currWidth = 0;
    for (Ogre::String::iterator iter = mMessageStr.begin(); iter != mMessageStr.end(); ++iter)
    {
        if (*iter == '\n')
        {
            ++lines;
            maxWidth = (currWidth > maxWidth) ? currWidth : maxWidth;
            currWidth = 0;
        }
        else
        {
            ++currWidth;
        }
    }
    maxWidth = (currWidth > maxWidth) ? currWidth : maxWidth; //For end of string (no newline).

    MyGUI::IntCoord coord;
    coord.height = ((lines + 2) * FONT_HEIGHT);
    coord.width = (maxWidth * FONT_WIDTH) + 5;

    coord.top = (winHeight - coord.height) - 60;
    coord.left = (winWidth - coord.width) / 2;

    //Show the message.
    mMessage = GlbVar.gui->createWidget<MyGUI::Edit>("EditStretch", coord, MyGUI::Align::HCenter | MyGUI::Align::Top, "Main");
    mMessage->setTextAlign(MyGUI::Align::HCenter | MyGUI::Align::Top);
    mMessage->setEditReadOnly(true);
    mMessage->setEditMultiLine(true);

    mMessage->setCaption("\n" + mMessageStr);

    if(!(mProperties.getValue("NGF_SERIALISED", 0, "no") == "yes"))
        mMessage->setAlpha(0); //For fade in (only if not deserialising).
}
//-------------------------------------------------------------------------------
MessageBox::~MessageBox()
{
    //Destroy message.
    GlbVar.gui->destroyWidget(mMessage);
}
//-------------------------------------------------------------------------------
void MessageBox::unpausedTick(const Ogre::FrameEvent &evt)
{
    if (mTimed)
    {
        //Timecheck.
        mTimeLeft -= evt.timeSinceLastFrame;
        if (mTimeLeft <= 0)
            GlbVar.goMgr->requestDestroy(getID());

        //Fade out.
        if (mTimeLeft <= MAX_ALPHA)
            mMessage->setAlpha(mTimeLeft);
        else
            mMessage->setAlpha(Util::clamp<Ogre::Real>(mMessage->getAlpha() + evt.timeSinceLastFrame, 0, MAX_ALPHA));
    }
    else
    {
        mMessage->setAlpha(Util::clamp<Ogre::Real>(mMessage->getAlpha() + evt.timeSinceLastFrame, 0, MAX_ALPHA));
    }
}
//-------------------------------------------------------------------------------
