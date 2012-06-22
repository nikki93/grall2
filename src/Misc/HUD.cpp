/*
=========================================
HUD.cpp
=========================================
*/

#include "Objects/Main/Player.h"

#include "Misc/HUD.h"

#define HUD_LAYER "Main"

#define TEXT_WIDTH 40 //Width of each text block.
#define TEXT_HEIGHT 50 //Height of each text block.
#define TEXT_BETWEEN_PAD 10 //Space between each text block.
#define TEXT_SIDE_PAD 55 //Space between text block and screen side edge.
#define TEXT_BOTTOM_PAD 50 //Space between text block and screen bottom edge.

#define ICON_WIDTH 32 //With of each icon.
#define ICON_HEIGHT 32 //Height of each icon.
#define ICON_BETWEEN_PAD 10 //Space between each icon.
#define ICON_BOTTOM_PAD 50 //Space between icon and screen bottom edge.

//Returns the position that the ID'th timer should have.
static inline Ogre::Vector2 timerPosition(int id)
{
    size_t winWidth = GlbVar.ogreWindow->getWidth();
    size_t winHeight = GlbVar.ogreWindow->getHeight();

    return Ogre::Vector2(
            winWidth - TEXT_SIDE_PAD - TEXT_WIDTH - ((TEXT_WIDTH + TEXT_BETWEEN_PAD) * id),
            winHeight - TEXT_BOTTOM_PAD - TEXT_HEIGHT
            );
}

//Returns the position that the i'th pickup display should have.
static inline Ogre::Vector2 pickupDisplayPosition(int i)
{
    size_t winHeight = GlbVar.ogreWindow->getHeight();

    return Ogre::Vector2(
            TEXT_SIDE_PAD + ((TEXT_WIDTH + TEXT_BETWEEN_PAD) * i),
            winHeight - TEXT_BOTTOM_PAD - TEXT_HEIGHT
            );
}

//Returns the position that the i'th icon should have.
static inline Ogre::Vector2 iconPosition(int i, int num)
{
    size_t winWidth = GlbVar.ogreWindow->getWidth();
    size_t winHeight = GlbVar.ogreWindow->getHeight();

    return Ogre::Vector2(
            ((winWidth - ((ICON_WIDTH + ICON_BETWEEN_PAD) * num - ICON_BETWEEN_PAD)) / 2) //First icon position.
            + (ICON_WIDTH + ICON_BETWEEN_PAD) * i, //Current icon position.
            winHeight - ICON_BOTTOM_PAD - ICON_HEIGHT
            );
}

//Creates a MyGUI::StaticText for HUD.
static inline MyGUI::StaticTextPtr createStaticText(const MyGUI::IntCoord &coord, MyGUI::Align align, const Ogre::ColourValue &colour = Ogre::ColourValue::White)
{
    MyGUI::StaticTextPtr txt = GlbVar.gui->createWidget<MyGUI::StaticText>("TextBox", coord, MyGUI::Align::Default, HUD_LAYER);

    align |= MyGUI::Align::Bottom;
    txt->setTextAlign(align);
    txt->setFontName("HUD");
    txt->setTextColour(MyGUI::Colour(colour.r, colour.g, colour.b, colour.a));

    return txt;
}

//--- HUD -----------------------------------------------------------------------
HUD::HUD()
{
    Ogre::Vector2 pos = timerPosition(0);
    mBonusTimer = createStaticText(MyGUI::IntCoord(pos.x, pos.y, TEXT_WIDTH, TEXT_HEIGHT), MyGUI::Align::Right, Ogre::ColourValue::White);
}
//-------------------------------------------------------------------------------
void HUD::clear()
{
    //Remove all timers.
    for (TimerMap::iterator iter = mTimers.begin(); iter != mTimers.end(); ++iter)
        delete iter->second;
    mTimers.clear();

    //Remove all pickup displays.
    for (PickupDisplayMap::iterator iter = mPickupDisplays.begin(); 
            iter != mPickupDisplays.end(); ++iter)
        delete iter->second;
    mPickupDisplays.clear();

    //Remove all icons.
    for (IconMap::iterator iter = mIcons.begin(); 
            iter != mIcons.end(); ++iter)
        delete iter->second;
    mIcons.clear();

    //Clear bonus timer.
    mBonusTimer ->setCaption("");
}
//-------------------------------------------------------------------------------
void HUD::tick(const Ogre::FrameEvent &evt)
{
    //Update each timer, removing it if it's run out.
    for (TimerMap::iterator iter = mTimers.begin(); iter != mTimers.end(); )
    {
        Timer *timer = iter->second;

        if (!timer->update(evt.timeSinceLastFrame))
        {
            delete iter->second;
            mTimers.erase(iter++);
        }
        else
            ++iter;
    }

    //Update each pickup display.
    int i = 0;
    for (PickupDisplayMap::iterator iter = mPickupDisplays.begin(); 
            iter != mPickupDisplays.end(); ++i, ++iter)
        iter->second->update(pickupDisplayPosition(i));

    //Update each icon.
    i = 0;
    int num = mIcons.size();
    for (IconMap::iterator iter = mIcons.begin(); 
            iter != mIcons.end(); ++i, ++iter)
        iter->second->update(iconPosition(i, num));

    //If there's a bonus time, show it, else don't.
    if (mBonusTimer)
        if (GlbVar.bonusTime > 0)
        {
            Ogre::Vector2 pos = timerPosition(0);
            mBonusTimer->setCoord(MyGUI::IntCoord(pos.x, pos.y, TEXT_WIDTH, TEXT_HEIGHT));
            mBonusTimer->setCaption(Ogre::StringConverter::toString((int) GlbVar.bonusTime));
        }
        else
            mBonusTimer->setCaption("");
}
//-------------------------------------------------------------------------------
int HUD::addTimer(Ogre::Real time, const Ogre::ColourValue &colour)
{
    //Find a free ID. ID 0 would be bonus time.
    int id = 1;
    for (; mTimers.find(id) != mTimers.end(); ++id);

    mTimers.insert(std::make_pair(id, new Timer(time, timerPosition(id), colour)));

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
void HUD::addPickupDisplay(const Ogre::String &type, const Ogre::ColourValue &colour)
{
    //If we don't already have a timer for this type, add it.
    std::pair<PickupDisplayMap::iterator, bool> ret = 
        mPickupDisplays.insert(std::make_pair(type, (PickupDisplay *) 0));

    if (ret.second)
        ret.first->second = new PickupDisplay(type, colour);
}
//-------------------------------------------------------------------------------
void HUD::removePickupDisplay(const Ogre::String &type)
{
    PickupDisplayMap::iterator iter = mPickupDisplays.find(type);

    if (iter != mPickupDisplays.end())
    {
        delete iter->second;
        mPickupDisplays.erase(iter);
    }
}
//-------------------------------------------------------------------------------
void HUD::setIcon(const Ogre::String &name, const Ogre::String &imageFile)
{
    std::pair<IconMap::iterator, bool> ret = mIcons.insert(std::make_pair(name, (Icon *) 0));

    if (ret.second) //If new element.
        ret.first->second = new Icon(imageFile);
    else
        ret.first->second->setImage(imageFile);
}
//-------------------------------------------------------------------------------
void HUD::removeIcon(const Ogre::String &name)
{
    IconMap::iterator iter = mIcons.find(name);

    if (iter != mIcons.end())
    {
        delete iter->second;
        mIcons.erase(iter);
    }
}
//-------------------------------------------------------------------------------

//--- Timer ------------------------------------------------------------------
HUD::Timer::Timer(Ogre::Real time, Ogre::Vector2 pos, const Ogre::ColourValue &colour)
    : mTime(time),
      mText(0)
{
    mText = createStaticText(MyGUI::IntCoord(pos.x, pos.y, TEXT_WIDTH, TEXT_HEIGHT), MyGUI::Align::Right, colour);
    mText->setCaption(Ogre::StringConverter::toString((int) mTime));
}
//-------------------------------------------------------------------------------
HUD::Timer::~Timer()
{
    if (mText)
        GlbVar.gui->destroyWidget(mText);
}
//-------------------------------------------------------------------------------
bool HUD::Timer::update(Ogre::Real elapsed)
{
    mTime -= elapsed;

    if (mTime <= 0)
        return false;

    mText->setCaption(Ogre::StringConverter::toString((int) mTime));

    return true;
}
//-------------------------------------------------------------------------------

//--- PickupDisplay -------------------------------------------------------------
HUD::PickupDisplay::PickupDisplay(const Ogre::String &type, const Ogre::ColourValue &colour)
    : mType(type)
{
    mText = createStaticText(MyGUI::IntCoord(-100,-100,0,0), MyGUI::Align::Left, colour);
    mText->setCaption(""); //Initially empty.
}
//-------------------------------------------------------------------------------
HUD::PickupDisplay::~PickupDisplay()
{
    GlbVar.gui->destroyWidget(mText);
}
//-------------------------------------------------------------------------------
void HUD::PickupDisplay::update(const Ogre::Vector2 &pos)
{
    mText->setCoord(MyGUI::IntCoord(pos.x, pos.y, TEXT_WIDTH, TEXT_HEIGHT));
    if (GlbVar.player)
    {
        Player *player = dynamic_cast<Player*>(GlbVar.player);
        if (player)
            mText->setCaption(Ogre::StringConverter::toString(player->getNumPickups(mType)));
    }
}
//-------------------------------------------------------------------------------

//--- Icon ----------------------------------------------------------------------
HUD::Icon::Icon(const Ogre::String &imageFile)
    : mTextureName(imageFile)
{
    mImage = GlbVar.gui->createWidget<MyGUI::StaticImage>("StaticImage", 
            MyGUI::IntCoord(-100,-100,ICON_WIDTH,ICON_HEIGHT), MyGUI::Align::Default, HUD_LAYER);
    mImage->setImageTexture(mTextureName);
}
//-------------------------------------------------------------------------------
HUD::Icon::~Icon()
{
    GlbVar.gui->destroyWidget(mImage);
}
//-------------------------------------------------------------------------------
void HUD::Icon::update(const Ogre::Vector2 &pos)
{
    mImage->setPosition(pos.x, pos.y);
}
//-------------------------------------------------------------------------------
void HUD::Icon::setImage(const Ogre::String &imageFile)
{
    mTextureName = imageFile;
    mImage->setImageTexture(mTextureName);
}
//-------------------------------------------------------------------------------
