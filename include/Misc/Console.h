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

class Console
{
    protected:
        MyGUI::WindowPtr mWindow;
        MyGUI::EditPtr mInputBox;
        MyGUI::EditPtr mOutputBox;

        bool mVisible;

    public:
        Console()
        {
            MyGUI::LayoutManager::getInstance().load("Console.layout");

            mWindow = GlbVar.gui->findWidget<MyGUI::Window>("win_console");

	    MyGUI::ButtonPtr runButton = GlbVar.gui->findWidget<MyGUI::Button>("but_consoleRun");
	    runButton->eventMouseButtonClick = MyGUI::newDelegate(this, &Console::_onClickRun);

	    mOutputBox = MyGUI::Gui::getInstancePtr()->findWidget<MyGUI::Edit>("edt_consoleOut");
	    mOutputBox->setTextAlign(MyGUI::Align::Top | MyGUI::Align::Left);
	    //mOutputBox->setFontName("Mono");

            mInputBox = MyGUI::Gui::getInstancePtr()->findWidget<MyGUI::Edit>("edt_consoleIn");
            mInputBox->setTextAlign(MyGUI::Align::Top | MyGUI::Align::Left);
	    //mInputBox->setFontName("Mono");

            mWindow->setVisible(false);
            mVisible = false;
        }

        ~Console()
        {
        }

        void keyPressed(OIS::KeyCode kc)
        {
            switch (kc)
            {
                case OIS::KC_F4:
                    setVisible(!isVisible());
                    break;
                case OIS::KC_F5:
                    if (isVisible())
                        _onClickRun(NULL);
                    break;
            }
        }

        inline void setVisible(bool visible)
        {
            mWindow->setVisible(visible);
        }

        inline bool isVisible()
        {
            return mWindow->isVisible();
        }

        void print(Ogre::String str)
        {
            mOutputBox->setCaption(mOutputBox->getCaption() + str);
        }

        //--- Internal stuff ----------------------------------------------------------

        void _onClickRun(MyGUI::WidgetPtr)
        {
            mInputBox = GlbVar.gui->findWidget<MyGUI::Edit>("edt_consoleIn");
            Ogre::String inStr = mInputBox->getOnlyText();
            PyRun_SimpleString(inStr.c_str());
        }

};

#endif
