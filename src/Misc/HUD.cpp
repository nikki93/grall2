/*
=========================================
HUD.cpp
=========================================
*/

#include "Misc/HUD.h"

#define HUD_LAYER "Main"

#define TIMER_WIDTH 40 //Width of each timer block.
#define TIMER_HEIGHT 50 //Height of each timer block.

#define TIMER_SIDE_PAD 55 //Space between first timer block and screen side edge.
#define TIMER_BOTTOM_PAD 50 //Space between timer blocks and screen bottom edge.
#define TIMER_BETWEEN_PAD 10 //Space between each timer block.

//Returns the position that the ID'th timer should have.
static inline Ogre::Vector2 timerPosition(int id)
{
    size_t winWidth = GlbVar.ogreWindow->getWidth();
    size_t winHeight = GlbVar.ogreWindow->getHeight();

    return Ogre::Vector2(
            winWidth - TIMER_SIDE_PAD - TIMER_WIDTH - ((TIMER_WIDTH + TIMER_BETWEEN_PAD) * id),
            winHeight - TIMER_BOTTOM_PAD - TIMER_HEIGHT
            );

    /* For left side timers.
    return Ogre::Vector2(
            TIMER_SIDE_PAD + (TIMER_WIDTH + TIMER_BETWEEN_PAD) * id, 
            winHeight - TIMER_BOTTOM_PAD - TIMER_HEIGHT
            );
    */
}

//Creates a MyGUI::StaticText for HUD.
static inline MyGUI::StaticTextPtr createStaticText(const MyGUI::IntCoord &coord, MyGUI::Align align, const Ogre::ColourValue &colour = Ogre::ColourValue::White)
{
    MyGUI::StaticTextPtr txt = GlbVar.gui->createWidget<MyGUI::StaticText>("StaticText", coord, MyGUI::Align::Default, HUD_LAYER);

    align |= MyGUI::Align::Bottom;
    txt->setTextAlign(align);
    txt->setFontName("HUD");
    txt->setTextColour(MyGUI::Colour(colour.r, colour.g, colour.b, colour.a));

    return txt;
}

//--- HUDTimer ------------------------------------------------------------------
HUD::HUDTimer::HUDTimer(Ogre::Real time, Ogre::Vector2 pos, const Ogre::ColourValue &colour)
    : mTime(time),
      mText(0)
{
    mText = createStaticText(MyGUI::IntCoord(pos.x, pos.y, TIMER_WIDTH, TIMER_HEIGHT), MyGUI::Align::Right, colour);
    mText->setCaption(Ogre::StringConverter::toString((int) mTime));
}
//-------------------------------------------------------------------------------
HUD::HUDTimer::~HUDTimer()
{
    if (mText)
        GlbVar.gui->destroyWidget(mText);
}
//-------------------------------------------------------------------------------
bool HUD::HUDTimer::update(Ogre::Real elapsed)
{
    mTime -= elapsed;

    if (mTime <= 0)
        return false;

    mText->setCaption(Ogre::StringConverter::toString((int) mTime));

    return true;
}
//-------------------------------------------------------------------------------

//--- HUD -----------------------------------------------------------------------
HUD::HUD()
{
    size_t winHeight = GlbVar.ogreWindow->getHeight();
    Ogre::Vector2 pos = timerPosition(0);
    mBonusTimer = createStaticText(MyGUI::IntCoord(pos.x, pos.y, TIMER_WIDTH, TIMER_HEIGHT), MyGUI::Align::Right, Ogre::ColourValue::White);
}
//-------------------------------------------------------------------------------
void HUD::clear()
{
    //Remove all timers.
    for (TimerMap::iterator iter = mTimers.begin(); iter != mTimers.end(); ++iter)
        delete iter->second;
    mTimers.clear();

    //Clear bonus timer.
    mBonusTimer ->setCaption("");
}
//-------------------------------------------------------------------------------
void HUD::tick(const Ogre::FrameEvent &evt)
{
    //Update each timer, removing it if it's run out.
    for (TimerMap::iterator iter = mTimers.begin(); iter != mTimers.end(); ++iter)
    {
        HUDTimer *timer = iter->second;

        if (!timer->update(evt.timeSinceLastFrame))
            removeTimer(iter->first);
    }

    //If there's a bonus time, show it, else don't.
    if (mBonusTimer)
        if (GlbVar.bonusTime > 0)
            mBonusTimer->setCaption(Ogre::StringConverter::toString((int) GlbVar.bonusTime));
        else
            mBonusTimer->setCaption("");
}
//-------------------------------------------------------------------------------
int HUD::addTimer(Ogre::Real time, const Ogre::ColourValue &colour)
{
    //Find a free ID. ID 0 would be bonus time.
    int id = 1;
    for (; mTimers.find(id) != mTimers.end(); ++id);

    mTimers.insert(std::pair<int, HUDTimer*>(id, new HUDTimer(time, timerPosition(id), colour)));

    return id;
}
//-------------------------------------------------------------------------------
void HUD::removeTimer(int id)
{
    TimerMap::iterator iter = mTimers.find(id);

    if (iter != mTimers.end())
    {
        delete iter->second;
        mTimers.erase(iter);
    }
}
//-------------------------------------------------------------------------------
