/*
=========================================
Slideshow.cpp
=========================================
*/

#define __SLIDESHOW_CPP__

#include "Objects/Misc/Slideshow.h"

//--- NGF events ----------------------------------------------------------------
Slideshow::Slideshow(Ogre::Vector3 pos, Ogre::Quaternion rot, NGF::ID id, NGF::PropertyList properties, Ogre::String name)
    : NGF::GameObject(pos, rot, id , properties, name),
      mCurrTime(0),
      mCurrInd(-1),
      mEnabled(true)
{
    Ogre::String ogreName = "id" + Ogre::StringConverter::toString(getID());
    addFlag("Slideshow");

    if (GlbVar.currSlideshow)
        GlbVar.goMgr->destroyObject(GlbVar.currSlideshow->getID());
    GlbVar.currSlideshow = this;

    if (!GlbVar.newLevel)
    {
        mEnabled = false;
        GlbVar.goMgr->requestDestroy(getID());
        return;
    }

    int winHeight = GlbVar.ogreWindow->getHeight();
    int winWidth = GlbVar.ogreWindow->getWidth();
    mBackground = GlbVar.gui->createWidget<MyGUI::StaticImage>("StaticImage", MyGUI::IntCoord(0,0,winWidth,winHeight), MyGUI::Align::Default, "Popup");
    mBackground->setImageTexture("fade.png");

    int imgHeight = Ogre::StringConverter::parseInt(mProperties.getValue("height", 0, "480"));
    int imgWidth = Ogre::StringConverter::parseInt(mProperties.getValue("width", 0, "640"));
    mImage = GlbVar.gui->createWidget<MyGUI::StaticImage>("StaticImage", MyGUI::IntCoord((winWidth - imgWidth) * 0.5,(winHeight - imgHeight) * 0.5,imgWidth,imgHeight), MyGUI::Align::Default, "Popup");

    GlbVar.paused = true;
}
//-------------------------------------------------------------------------------
Slideshow::~Slideshow()
{
    GlbVar.gui->destroyWidget(mImage);
    GlbVar.gui->destroyWidget(mBackground);
}
//-------------------------------------------------------------------------------
void Slideshow::unpausedTick(const Ogre::FrameEvent &evt)
{
    updateAlarms(evt.timeSinceLastFrame);
}
//-------------------------------------------------------------------------------
void Slideshow::pausedTick(const Ogre::FrameEvent &evt)
{
    updateAlarms(evt.timeSinceLastFrame);
}
//-------------------------------------------------------------------------------
NGF::MessageReply Slideshow::receiveMessage(NGF::Message msg)
{
    if (!mEnabled)
        NGF_NO_REPLY();

    switch (msg.code)
    {
        case MSG_ADDSLIDE:
            if (++mCurrInd > 0)
                mSlides.push_back(msg.getParam<Ogre::String>(1));
            else
                mImage->setImageTexture(msg.getParam<Ogre::String>(1));

            mCurrTime += msg.getParam<Ogre::Real>(0);
            setAlarm(mCurrTime, mCurrInd);
            NGF_NO_REPLY();
    }
    NGF_NO_REPLY();
}
//-------------------------------------------------------------------------------
void Slideshow::alarm(Alarm a)
{
    if (a < mSlides.size())
        mImage->setImageTexture(mSlides[a]);
    else
    {
        GlbVar.paused = false;
        GlbVar.goMgr->requestDestroy(getID());
    }
}
//-------------------------------------------------------------------------------
