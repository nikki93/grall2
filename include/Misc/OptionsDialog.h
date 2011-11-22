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

        MyGUI::HScrollPtr mTurningScroll;
        MyGUI::StaticTextPtr mTurningText;
        MyGUI::HScrollPtr mUpDownScroll;
        MyGUI::StaticTextPtr mUpDownText;
        MyGUI::ButtonPtr mInvertMouseCheckBox;
        MyGUI::ComboBoxPtr mResolutionsBox;
        MyGUI::ButtonPtr mFullscreenCheckBox;
        MyGUI::ButtonPtr mLightingCheckBox;
        MyGUI::ButtonPtr mNormalMapCheckBox;
        MyGUI::ButtonPtr mParallaxMapCheckBox;
        MyGUI::ComboBoxPtr mRenderersBox;

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
        void onSelectResolution(MyGUI::ComboBoxPtr, size_t index);
        void onClickFullscreen(MyGUI::WidgetPtr);
        void onClickLighting(MyGUI::WidgetPtr);
        void onClickNormalMap(MyGUI::WidgetPtr);
        void onClickParallaxMap(MyGUI::WidgetPtr);
        void onClickGraphicsApply(MyGUI::WidgetPtr);

        //--- Visibility ---------------------------------------------------------------
        void setVisible(bool visible);
        bool getVisible()
        {
            return mWindow->getVisible();
        }
};

#endif
