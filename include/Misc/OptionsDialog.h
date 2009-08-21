/*
 * =====================================================================================
 *
 *       Filename:  OptionsDialog.h
 *
 *    Description:  The options dialog. Beware: Messy code. :-(
 *
 *        Created:  Friday 21 August 2009 03:55:47  IST
 *       Compiler:  gcc
 *
 *         Author:  Nikhilesh (nikki)
 *
 * =====================================================================================
 */

#ifndef __OPTIONSDIALOG_H__
#define __OPTIONSDIALOG_H__

#include "Globals.h"

class OptionsDialog
{
    protected:
        MyGUI::WindowPtr mWindow;

        bool mListeningForKey; //Whether we're listening.
        Ogre::String mCurrKey; //The control ('forward', 'dimensionSwitch', etc.) we're listening for.

    public:
        OptionsDialog();
        ~OptionsDialog();

        void tick();
        void stopListeningForKey();

        //--- Events -------------------------------------------------------------------
        void onClickChangeKey(MyGUI::WidgetPtr);
        void onClickOK(MyGUI::WidgetPtr);
        void keyPressed(OIS::KeyCode kc);

        //--- Visibility ---------------------------------------------------------------
        void setVisible(bool visible)
        {
            mWindow->setVisibleSmooth(visible);
        }
        bool getVisible()
        {
            return mWindow->isVisible();
        }
};

#endif
