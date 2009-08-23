/*
=========================================
MainMenu.cpp
=========================================
*/

#include "Worlds/MainMenu.h"
#include "Worlds/Level.h"

#define LOGO_WIDTH 480
#define LOGO_HEIGHT 151

/*
 * ==============================================================================
 * MainMenu
 * ==============================================================================
 */

//-------------------------------------------------------------------------------
MainMenu::MainMenu()
{
}
//-------------------------------------------------------------------------------
void MainMenu::init()
{
    int winHeight = GlbVar.ogreWindow->getHeight();
    int winWidth = GlbVar.ogreWindow->getWidth();

    //Background, logo.
    mBackground = GlbVar.gui->createWidget<MyGUI::StaticImage>("StaticImage", MyGUI::IntCoord(0,0,winWidth,winHeight), MyGUI::Align::Default, "Back");
    mBackground->setImageTexture("MainMenuBgr.png");

    int logoX = winWidth - (LOGO_WIDTH + 5);
    int logoY = winHeight - (LOGO_HEIGHT + 5);
    mLogo = GlbVar.gui->createWidget<MyGUI::StaticImage>("StaticImage", MyGUI::IntCoord(logoX, logoY, LOGO_WIDTH, LOGO_HEIGHT), MyGUI::Align::Default, "Back");
    mLogo->setImageTexture("MainMenuLogo.png");

    //Get the Window.
    MyGUI::LayoutManager::getInstance().load("MainMenu.layout");
    mWindow = GlbVar.gui->findWidget<MyGUI::Window>("win_mainMenu");

    //Buttons, events.
    MyGUI::ButtonPtr button;
    button = GlbVar.gui->findWidget<MyGUI::Button>("but_newGame");
    button->eventMouseButtonClick = MyGUI::newDelegate(this, &MainMenu::onClickNewGame);

    button = GlbVar.gui->findWidget<MyGUI::Button>("but_continueGame");
    button->eventMouseButtonClick = MyGUI::newDelegate(this, &MainMenu::onClickContinueGame);
    if (GlbVar.records.firstTime)
        button->setEnabled(false);

    button = GlbVar.gui->findWidget<MyGUI::Button>("but_levelSelect");
    button->eventMouseButtonClick = MyGUI::newDelegate(this, &MainMenu::onClickLevelSelect);
    if (GlbVar.records.firstTime)
        button->setEnabled(false);

    button = GlbVar.gui->findWidget<MyGUI::Button>("but_options");
    button->eventMouseButtonClick = MyGUI::newDelegate(this, &MainMenu::onClickOptions);

    button = GlbVar.gui->findWidget<MyGUI::Button>("but_credits");
    button->eventMouseButtonClick = MyGUI::newDelegate(this, &MainMenu::onClickCredits);

    button = GlbVar.gui->findWidget<MyGUI::Button>("but_quit");
    button->eventMouseButtonClick = MyGUI::newDelegate(this, &MainMenu::onClickQuit);

    //Window properties.
    //Children don't inherit alpha.
    MyGUI::EnumeratorWidgetPtr iter = mWindow->getEnumerator();
    while (iter.next())
    {
        iter.current()->setInheritsAlpha(false);
    }

    mWindow->setAlpha(0);
    mWindow->setPosition(30,5);

    //Create child interfaces.
    mLevelSelect = new LevelSelect();
    MyGUI::LayoutManager::getInstance().load("Credits.layout");
    mCreditsWindow = GlbVar.gui->findWidget<MyGUI::Window>("win_credits");
    mCreditsWindow->setVisible(false);

    int height = mCreditsWindow->getHeight();
    int width = mCreditsWindow->getWidth();
    mCreditsWindow->setCoord(MyGUI::IntCoord((winWidth - width)*0.5, (winHeight - height)*0.5, width, height));

    button = GlbVar.gui->findWidget<MyGUI::Button>("but_cr_ok");
    button->eventMouseButtonClick = MyGUI::newDelegate(this, &MainMenu::onClickCloseCredits);

    Ogre::String creditsStr = 
        "Everything:\n"
        "  Nikhilesh (nikki)";

    MyGUI::EditPtr credits = GlbVar.gui->findWidget<MyGUI::Edit>("edt_cr_credits");
    credits->setOnlyText(creditsStr);
    credits->setTextAlign(MyGUI::Align::Left | MyGUI::Align::Top);
}
//-------------------------------------------------------------------------------
void MainMenu::tick(const Ogre::FrameEvent &evt)
{
    mWindow->setPosition(30,5);
}
//-------------------------------------------------------------------------------
void MainMenu::stop()
{
    delete mLevelSelect;
    GlbVar.optionsDialog->setVisible(false);
    GlbVar.gui->destroyWidget(mWindow);
    GlbVar.gui->destroyWidget(mCreditsWindow);
    GlbVar.gui->destroyWidget(mLogo);
    GlbVar.gui->destroyWidget(mBackground);
}
//-------------------------------------------------------------------------------

//--- Events --------------------------------------------------------------------
void MainMenu::onClickNewGame(MyGUI::WidgetPtr)
{
    if (GlbVar.records.firstTime)
    {
        onConfirmNewGame(0, MyGUI::MessageBoxStyle::Yes); //Directly 'yes' if no earlier game. :P
    }
    else
    {
        Ogre::String warning = "Are you sure you want to start a new game? This will erase\n"
            "your previous progress!";

        MyGUI::MessagePtr message = MyGUI::Message::createMessageBox("Message", "Warning!",
                warning, MyGUI::MessageBoxStyle::Yes | MyGUI::MessageBoxStyle::No);
        message->eventMessageBoxResult = MyGUI::newDelegate(this, &MainMenu::onConfirmNewGame);
    }
}
void MainMenu::onConfirmNewGame(MyGUI::MessagePtr, MyGUI::MessageBoxStyle result)
{
    if (result == MyGUI::MessageBoxStyle::Yes)
    {
        //Clear user records.
        clearRecord();
        GlbVar.records.firstTime = false;
        GlbVar.woMgr->gotoWorld(GlbVar.records.highestLevelIndex); //Highest level is now first.
    }
}
//-------------------------------------------------------------------------------
void MainMenu::onClickContinueGame(MyGUI::WidgetPtr)
{
    //Continue from latest world.
    GlbVar.woMgr->gotoWorld(GlbVar.records.highestLevelIndex);
}
//-------------------------------------------------------------------------------
void MainMenu::onClickLevelSelect(MyGUI::WidgetPtr)
{
    mLevelSelect->show();
}
//-------------------------------------------------------------------------------
void MainMenu::onClickQuit(MyGUI::WidgetPtr)
{
    GlbVar.woMgr->shutdown();
}
//-------------------------------------------------------------------------------
void MainMenu::onClickCredits(MyGUI::WidgetPtr)
{
    mCreditsWindow->setVisibleSmooth(true);
}
//-------------------------------------------------------------------------------
void MainMenu::onClickCloseCredits(MyGUI::WidgetPtr)
{
    mCreditsWindow->setVisibleSmooth(false);
}
//-------------------------------------------------------------------------------
void MainMenu::onClickOptions(MyGUI::WidgetPtr)
{
    GlbVar.optionsDialog->setVisible(true);
}
//-------------------------------------------------------------------------------

/*
 * ==============================================================================
 * LevelSelect
 * ==============================================================================
 */

//-------------------------------------------------------------------------------
LevelSelect::LevelSelect()
    : mCurrentLevelIndex(GlbVar.firstLevel)
{
    MyGUI::LayoutManager::getInstance().load("LevelSelect.layout");
    mWindow = GlbVar.gui->findWidget<MyGUI::Window>("win_levelSelect");
    mWindow->setVisible(false);

    //Center it.
    int winHeight = GlbVar.ogreWindow->getHeight();
    int winWidth = GlbVar.ogreWindow->getWidth();
    int height = mWindow->getHeight();
    int width = mWindow->getWidth();
    mWindow->setCoord(MyGUI::IntCoord((winWidth - width)*0.5, (winHeight - height)*0.5, width, height));

    //Stuff, events.
    MyGUI::ButtonPtr button;
    button = GlbVar.gui->findWidget<MyGUI::Button>("but_ls_cancel");
    button->eventMouseButtonClick = MyGUI::newDelegate(this, &LevelSelect::onClickCancel);

    button = GlbVar.gui->findWidget<MyGUI::Button>("but_ls_removeCheckpoint");
    button->eventMouseButtonClick = MyGUI::newDelegate(this, &LevelSelect::onClickRemoveCheckpoint);

    button = GlbVar.gui->findWidget<MyGUI::Button>("but_ls_play");
    button->eventMouseButtonClick = MyGUI::newDelegate(this, &LevelSelect::onClickPlay);

    mList = GlbVar.gui->findWidget<MyGUI::List>("lst_ls_levelList");
    populateLevelList();
    mList->setIndexSelected(0);
    mList->eventListMouseItemActivate = MyGUI::newDelegate(this, &LevelSelect::onSelectLevel);
    mList->eventListSelectAccept = MyGUI::newDelegate(this, &LevelSelect::onSelectLevel);
    updateLevelInfo();
}
//-------------------------------------------------------------------------------
LevelSelect::~LevelSelect()
{
    GlbVar.gui->destroyWidget(mWindow);
}
//-------------------------------------------------------------------------------
void LevelSelect::populateLevelList()
{
    for (unsigned int i = GlbVar.firstLevel, n = 1; i <= GlbVar.records.highestLevelIndex; ++i, ++n)
    {
        Ogre::String lvlCaption = dynamic_cast<Level*>(GlbVar.woMgr->getWorldAt(i))->getCaption();
        Ogre::String caption = "Level " + Ogre::StringConverter::toString(n) + " - '" + lvlCaption + "'";
        mList->insertItemAt(n - 1, caption);
    }
}
//-------------------------------------------------------------------------------
void LevelSelect::updateLevelInfo()
{
    Level *lvl = dynamic_cast<Level*>(GlbVar.woMgr->getWorldAt(mCurrentLevelIndex));
    GlbVar.gui->findWidget<MyGUI::StaticText>("txt_ls_caption")->setCaption(lvl->getCaption());

    const Globals::Records::Record &rec = GlbVar.records.recordMap[worldNumToLevelNum(mCurrentLevelIndex)];

    if (rec.completed)
        GlbVar.gui->findWidget<MyGUI::StaticText>("txt_ls_score")->setCaption(Ogre::StringConverter::toString(rec.score));
    else
        GlbVar.gui->findWidget<MyGUI::StaticText>("txt_ls_score")->setCaption("n/a");

    GlbVar.gui->findWidget<MyGUI::StaticText>("txt_ls_losses")->setCaption(Ogre::StringConverter::toString(rec.losses));

    if (getRecordFromLevelNum(worldNumToLevelNum(mCurrentLevelIndex)).checkpoint)
    {
        GlbVar.gui->findWidget<MyGUI::StaticText>("txt_ls_checkpoint")->setCaption("Yes");
        GlbVar.gui->findWidget<MyGUI::StaticText>("txt_ls_checkpoint")->setTextColour(MyGUI::Colour(0.3,0.95,0.3));
        GlbVar.gui->findWidget<MyGUI::Button>("but_ls_removeCheckpoint")->setEnabled(true);
    }
    else
    {
        GlbVar.gui->findWidget<MyGUI::StaticText>("txt_ls_checkpoint")->setCaption("No");
        GlbVar.gui->findWidget<MyGUI::StaticText>("txt_ls_checkpoint")->setTextColour(MyGUI::Colour(1,1,1));
        GlbVar.gui->findWidget<MyGUI::Button>("but_ls_removeCheckpoint")->setEnabled(false);
    }
}
//-------------------------------------------------------------------------------
void LevelSelect::hide()
{
    mWindow->setVisibleSmooth(false);
}
//-------------------------------------------------------------------------------
void LevelSelect::show()
{
    mWindow->setVisibleSmooth(true);
}
//-------------------------------------------------------------------------------

//--- Events --------------------------------------------------------------------
void LevelSelect::onClickCancel(MyGUI::WidgetPtr)
{
    hide();
}
//-------------------------------------------------------------------------------
void LevelSelect::onSelectLevel(MyGUI::ListPtr sender, size_t index)
{
    mCurrentLevelIndex = GlbVar.firstLevel + index;
    updateLevelInfo();
}
//-------------------------------------------------------------------------------
void LevelSelect::onClickRemoveCheckpoint(MyGUI::WidgetPtr)
{
    Ogre::String warning = "Are you sure you want remove the checkpoint? This will erase\n"
                           "your progress in the level!";

    MyGUI::MessagePtr message = MyGUI::Message::createMessageBox("Message", "Warning!",
            warning, MyGUI::MessageBoxStyle::Yes | MyGUI::MessageBoxStyle::No);
    message->eventMessageBoxResult = MyGUI::newDelegate(this, &LevelSelect::onConfirmRemoveCheckpoint);
}
void LevelSelect::onConfirmRemoveCheckpoint(MyGUI::MessagePtr, MyGUI::MessageBoxStyle result)
{
    if (result == MyGUI::MessageBoxStyle::Yes)
    {
        getRecordFromLevelNum(worldNumToLevelNum(mCurrentLevelIndex)).checkpoint = false;
        updateLevelInfo();
    }
}
//-------------------------------------------------------------------------------
void LevelSelect::onClickPlay(MyGUI::WidgetPtr)
{
    GlbVar.woMgr->gotoWorld(mCurrentLevelIndex);
}
//-------------------------------------------------------------------------------
