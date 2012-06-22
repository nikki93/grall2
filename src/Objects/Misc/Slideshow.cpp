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
      mBackground(0),
      mImage(0)
{
    Ogre::String ogreName = "id" + Ogre::StringConverter::toString(getID());
    addFlag("Slideshow");

    if (GlbVar.currSlideshow)
        GlbVar.goMgr->destroyObject(GlbVar.currSlideshow->getID());
    GlbVar.currSlideshow = this;

    int winHeight = GlbVar.ogreWindow->getHeight();
    int winWidth = GlbVar.ogreWindow->getWidth();
    mBackground = GlbVar.gui->createWidget<MyGUI::StaticImage>("StaticImage", MyGUI::IntCoord(0,0,winWidth,winHeight), MyGUI::Align::Default, "Popup");
    mBackground->setImageTexture("fade.png");

    mImgHeight = Ogre::StringConverter::parseInt(mProperties.getValue("height", 0, "480"));
    mImgWidth = Ogre::StringConverter::parseInt(mProperties.getValue("width", 0, "640"));
    mImage = GlbVar.gui->createWidget<MyGUI::StaticImage>("StaticImage", 
            MyGUI::IntCoord((winWidth - mImgWidth) * 0.5,(winHeight - mImgHeight) * 0.5,mImgWidth,mImgHeight), 
            MyGUI::Align::Default, "Popup");
}
//-------------------------------------------------------------------------------
Slideshow::~Slideshow()
{
    if (mImage)
        GlbVar.gui->destroyWidget(mImage);
    if (mBackground)
        GlbVar.gui->destroyWidget(mBackground);
}
//-------------------------------------------------------------------------------
void Slideshow::unpausedTick(const Ogre::FrameEvent &evt)
{
    updateCoords();
    GlbVar.paused = true;
}
//-------------------------------------------------------------------------------
void Slideshow::pausedTick(const Ogre::FrameEvent &evt)
{
    updateCoords();
    updateAlarms(evt.timeSinceLastFrame);
}
//-------------------------------------------------------------------------------
void Slideshow::updateCoords()
{
    int winHeight = GlbVar.ogreWindow->getHeight();
    int winWidth = GlbVar.ogreWindow->getWidth();
    mBackground->setCoord(MyGUI::IntCoord(0,0,winWidth,winHeight));
    mImage->setPosition(MyGUI::IntPoint((winWidth - mImgWidth) * 0.5, (winHeight - mImgHeight) * 0.5));
}
//-------------------------------------------------------------------------------
NGF::MessageReply Slideshow::receiveMessage(NGF::Message msg)
{
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
