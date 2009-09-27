/*
=========================================
Level.cpp
=========================================
*/

#include "Worlds/Level.h"

#define OPENING_TEXT_TIME 5
#define OPENING_TEXT_FADE_TIME 2

//-------------------------------------------------------------------------------
Level::Level(unsigned int worldNum, Ogre::String ngfName, Ogre::String caption)
    : mNgfName(ngfName),
      mCaption(caption),
      mWorldNum(worldNum)
{
}
//-------------------------------------------------------------------------------
void Level::init()
{
    unsigned int levelInd = Util::worldNumToLevelNum(mWorldNum);

    //Just for fun. :P
    LOG("On to level: " + Ogre::StringConverter::toString(Util::worldNumToLevelNum(mWorldNum)) + ", NGF: " + mNgfName + ", Caption: " + mCaption + "!");

    //If higher than highest level, then highest level is this (user went to new level).
    if (mWorldNum > GlbVar.records.highestLevelIndex)
        GlbVar.records.highestLevelIndex = mWorldNum;

    //Create the GameObjects! If level 'NULL' (User-made), then we ask.
    if (mNgfName != "NULL")
    {
        startLevel();
    }
    else
    {
        GlbVar.gui->showPointer();

        //Just in case some smartypants tries to save before we load.
        mNgfName = "NULL";

        //Show the 'Load User Level' dialog.
        MyGUI::LayoutManager::getInstance().load("LoadUserLevel.layout");

        //Tell the button to tell us.
        MyGUI::ButtonPtr button = GlbVar.gui->findWidget<MyGUI::Button>("but_loadUserLevel");
        button->eventMouseButtonClick = MyGUI::newDelegate(this, &Level::onClickLoadUserLevel);

        //Populate the user level list.
        MyGUI::ComboBox *list = GlbVar.gui->findWidget<MyGUI::ComboBox>("cmb_userLevel");

        for (std::vector<Ogre::String>::iterator iter = GlbVar.userNgfNames.begin(); iter != GlbVar.userNgfNames.end(); ++iter)
            list->addItem(*iter);

        list->insertItemAt(0, "<none>");
        list->setIndexSelected(0);

        //Center the window.
        MyGUI::Window *win = GlbVar.gui->findWidget<MyGUI::Window>("win_userLevel");
        int winHeight = GlbVar.ogreWindow->getHeight();
        int winWidth = GlbVar.ogreWindow->getWidth();
        int height = win->getHeight();
        int width = win->getWidth();
        win->setCoord(MyGUI::IntCoord((winWidth - width)*0.5, (winHeight - height)*0.5, width, height));
    }
}
//-------------------------------------------------------------------------------
void Level::tick(const Ogre::FrameEvent &evt)
{
    //Part of the anti evt.timeSinceLastFrame hack.
    GlbVar.paused = false;
    
    //Some key stuff.
    if (Util::isKeyDown(OIS::KC_N))
        winLevel();
    if (Util::isKeyDown(OIS::KC_ESCAPE))
    {
        //In case some fades have been started, we stop 'em.
        GlbVar.fader->abortFade(0);
        Util::gotoWorld(0);
    }
}
//-------------------------------------------------------------------------------
void Level::stop()
{
    GlbVar.goMgr->sendMessage(GlbVar.controller, NGF_MESSAGE(MSG_LEVELSTOP));
    Util::clearLevel();

    if (mNgfName == "NULL")
        GlbVar.gui->findWidget<MyGUI::Window>("win_userLevel")->setVisible(false);
}
//-------------------------------------------------------------------------------
void Level::startLevel()
{
    //No stuff blocking our view.
    GlbVar.gui->hidePointer();

    if (GlbVar.loadGame && Util::getRecordFromLevelNum(Util::worldNumToLevelNum(mWorldNum)).checkpoint)
    {
        //If we're loading games, and savefile exists, load it.
        Util::deserialise(Util::saveName(mWorldNum));
    }
    else
    {
        //Otherwise read in the level from the .ngf file.
        Util::loadNgf(mNgfName);
        GlbVar.goMgr->sendMessage(GlbVar.controller, NGF_MESSAGE(MSG_LEVELSTART));
    }

    //To save from load-frame evt.timeSinceLastFrame spikes.
    GlbVar.paused = true;
}
//-------------------------------------------------------------------------------
void Level::onClickLoadUserLevel(MyGUI::WidgetPtr)
{
    MyGUI::ComboBox *list = GlbVar.gui->findWidget<MyGUI::ComboBox>("cmb_userLevel");
    Ogre::String name = list->getItemNameAt(list->getIndexSelected());

    if (name == "<none>")
        return;

    mCaption = (mNgfName = name);
    GlbVar.gui->findWidget<MyGUI::Window>("win_userLevel")->setVisible(false);
    startLevel();
}
//-------------------------------------------------------------------------------
