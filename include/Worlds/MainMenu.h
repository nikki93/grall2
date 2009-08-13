/*
 * =====================================================================================
 *
 *       Filename:  MainMenu.h
 *
 *    Description:  The main menu screen!
 *
 *        Created:  Wednesday 12 August 2009 05:10:55  IST
 *       Compiler:  gcc
 *
 *         Author:  Nikhilesh (nikki)
 *
 * =====================================================================================
 */

#ifndef __MAINMENU_H__
#define __MAINMENU_H__

#include "Globals.h"

class LevelSelect;

class MainMenu : 
    public NGF::World
{
    protected:
        MyGUI::WindowPtr mWindow;
        LevelSelect *mLevelSelect;

    public:
        MainMenu();
        ~MainMenu() {}

        void init();
        void tick(const Ogre::FrameEvent &evt);
        void stop();

        //--- Events -------------------------------------------------------------------
        void onClickNewGame(MyGUI::WidgetPtr);
        void onConfirmNewGame(MyGUI::MessagePtr, MyGUI::MessageBoxStyle result);
        void onClickContinueGame(MyGUI::WidgetPtr);
        void onClickLevelSelect(MyGUI::WidgetPtr);
        void onClickQuit(MyGUI::WidgetPtr);
};

class LevelSelect
{
    protected:
        MyGUI::WindowPtr mWindow;
        MyGUI::ListPtr mList;

        unsigned int mCurrentLevelIndex;

        void populateLevelList();

    public:
        LevelSelect();
        ~LevelSelect();

        void tick();

        void hide();
        void show();

        //--- Events -------------------------------------------------------------------
        void onClickCancel(MyGUI::WidgetPtr);
        void onSelectLevel(MyGUI::ListPtr sender, size_t index);
};

#endif
