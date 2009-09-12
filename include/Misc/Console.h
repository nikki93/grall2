/*
 * =====================================================================================
 *
 *       Filename:  Console.h
 *
 *    Description:  Handles the Python console window.
 *
 *        Created:  05/07/2009 05:52:47 PM
 *       Compiler:  gcc
 *
 *         Author:  Nikhilesh (nikki)
 *
 * =====================================================================================
 */

#ifndef __CONSOLE_H__
#define __CONSOLE_H__

#define MAX_HISTORY_SIZE 20

class Console
{
    protected:
        MyGUI::WindowPtr mWindow;
        MyGUI::EditPtr mInputBox;
        MyGUI::EditPtr mOutputBox;

        bool mVisible;

        std::deque<Ogre::String> mHistory;
        std::deque<Ogre::String>::iterator mHistoryIter;

        void insertInHistory(Ogre::String str)
        {
            mHistory.push_back(str);
            mHistoryIter = mHistory.end();

            //We have a limit bud! :P
            if (mHistory.size() > MAX_HISTORY_SIZE)
                mHistory.pop_front();
        }

        void backInHistory()
        {
            /*
            Ogre::String inStr = GlbVar.gui->findWidget<MyGUI::Edit>("edt_consoleIn")->getOnlyText();
            //If it's new, add it so it's the last.
            if (mHistoryIter == mHistory.end() || (inStr != *mHistoryIter))
                insertInHistory(inStr);
            */

            if (mHistoryIter != mHistory.begin())
                --mHistoryIter;
            if (mHistoryIter != mHistory.end())
                mInputBox->setOnlyText(*mHistoryIter);
            else
                GlbVar.gui->findWidget<MyGUI::Edit>("edt_consoleIn")->setOnlyText("");
        }

        void forwardInHistory()
        {
            if (mHistoryIter != mHistory.end())
                ++mHistoryIter;
            if (mHistoryIter != mHistory.end())
                mInputBox->setOnlyText(*mHistoryIter);
            else
                GlbVar.gui->findWidget<MyGUI::Edit>("edt_consoleIn")->setOnlyText("");
        }

    public:
        Console()
        {
            mHistoryIter = mHistory.begin();
            MyGUI::LayoutManager::getInstance().load("Console.layout");

            mWindow = GlbVar.gui->findWidget<MyGUI::Window>("win_console");
            mWindow->setAlpha(0.75);

	    mOutputBox = MyGUI::Gui::getInstancePtr()->findWidget<MyGUI::Edit>("edt_consoleOut");
	    mOutputBox->setTextAlign(MyGUI::Align::Top | MyGUI::Align::Left);
	    //mOutputBox->setFontName("Mono");

            mInputBox = MyGUI::Gui::getInstancePtr()->findWidget<MyGUI::Edit>("edt_consoleIn");
            mInputBox->setTextAlign(MyGUI::Align::Top | MyGUI::Align::Left);
	    //mInputBox->setFontName("Mono");

	    MyGUI::ButtonPtr button = GlbVar.gui->findWidget<MyGUI::Button>("but_consoleRun");
	    button->eventMouseButtonClick = MyGUI::newDelegate(this, &Console::_onClickRun);
            button = GlbVar.gui->findWidget<MyGUI::Button>("but_consoleClose");
	    button->eventMouseButtonClick = MyGUI::newDelegate(this, &Console::_onClickClose);

            mWindow->setVisible(false);
            mVisible = false;
        }

        ~Console()
        {
            GlbVar.gui->destroyWidget(mWindow);
        }

        void keyPressed(OIS::KeyCode kc)
        {
            switch (kc)
            {
                case OIS::KC_F3:
                    setVisible(!isVisible());
                    break;
                case OIS::KC_F4:
                    if (isVisible())
                        _onClickRun(NULL);
                    forwardInHistory();
                case OIS::KC_PGUP:
                    backInHistory();
                    break;
                case OIS::KC_PGDOWN:
                    forwardInHistory();
                    break;
            }
        }

        inline void setVisible(bool visible)
        {
            mWindow->setVisibleSmooth(visible);
            if (visible)
                MyGUI::InputManager::getInstance().setKeyFocusWidget(mInputBox);
            else
                MyGUI::InputManager::getInstance().setKeyFocusWidget(NULL);
        }

        inline bool isVisible()
        {
            return mWindow->isVisible();
        }

        void print(Ogre::String str)
        {
            mOutputBox->setCaption(mOutputBox->getCaption() + str);
            LOG(str);
        }

        //--- Button events -----------------------------------------------------------

        void _onClickRun(MyGUI::WidgetPtr)
        {
            MyGUI::EditPtr input = GlbVar.gui->findWidget<MyGUI::Edit>("edt_consoleIn");
            Ogre::String inStr = input->getOnlyText();

            PyRun_SimpleString("import Ngf");
            PyRun_SimpleString("import GraLL2");
            PyRun_SimpleString(inStr.c_str());

            if (mHistory.empty() || inStr != mHistory.back())
                insertInHistory(inStr);
        }

        void _onClickClose(MyGUI::WidgetPtr)
        {
            setVisible(false);
        }
};

#endif
