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

//Quantization of slider values.
#define SLIDER_QUANTUM 0.02

#include "Globals.h"

class OptionsDialog
{
    protected:
        MyGUI::WindowPtr mWindow;

        bool mListeningForKey; //Whether we're listening.
        Ogre::String mCurrKey; //The control ('forward', 'dimensionSwitch', etc.) we're listening for.

        MyGUI::StaticTextPtr mPressKeyText;

        MyGUI::HScroll *mTurningScroll;
        MyGUI::StaticTextPtr mTurningText;
        MyGUI::HScroll *mUpDownScroll;
        MyGUI::StaticTextPtr mUpDownText;
        MyGUI::ButtonPtr mInvertMouseCheckBox;

    public:
        OptionsDialog();
        ~OptionsDialog();

        void tick();
        void stopListeningForKey();
        void updateDisplay();

        //--- Events -------------------------------------------------------------------
        void keyPressed(OIS::KeyCode kc);
        void onClickChangeKey(MyGUI::WidgetPtr);
        void onClickOK(MyGUI::WidgetPtr);
        void onClickInvertMouse(MyGUI::WidgetPtr);

        //--- Visibility ---------------------------------------------------------------
        void setVisible(bool visible)
        {
            mWindow->setVisibleSmooth(visible);

            if (visible)
            {
                mTurningScroll->setScrollPosition((GlbVar.settings.controls.turningSensitivity / SLIDER_QUANTUM) - 1);
                mUpDownScroll->setScrollPosition((GlbVar.settings.controls.upDownSensitivity / SLIDER_QUANTUM) - 1);
            }
        }
        bool getVisible()
        {
            return mWindow->isVisible();
        }
};

#endif
