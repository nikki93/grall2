/*
=========================================
OptionsDialog.cpp
=========================================
*/

#include "Misc/OptionsDialog.h"

//-------------------------------------------------------------------------------
OptionsDialog::OptionsDialog()
    : mCurrKey(""),
      mListeningForKey(false)
{
    MyGUI::LayoutManager::getInstance().load("Options.layout");

    mWindow = GlbVar.gui->findWidget<MyGUI::Window>("win_options");
    mWindow->setVisible(false);

    //Save some widgets.
    mPressKeyText = GlbVar.gui->findWidget<MyGUI::StaticText>("o_c_txt_pressKey");

    mTurningScroll = GlbVar.gui->findWidget<MyGUI::HScroll>("o_c_sli_turningSensitivity");
    mTurningText = GlbVar.gui->findWidget<MyGUI::StaticText>("o_c_txt_turningSensitivity");
    mUpDownScroll = GlbVar.gui->findWidget<MyGUI::HScroll>("o_c_sli_upDownSensitivity");
    mUpDownText = GlbVar.gui->findWidget<MyGUI::StaticText>("o_c_txt_upDownSensitivity");
    mInvertMouseCheckBox = GlbVar.gui->findWidget<MyGUI::Button>("o_c_chk_invertMouse");
    mInvertMouseCheckBox->eventMouseButtonClick = MyGUI::newDelegate(this, &OptionsDialog::onClickInvertMouse);

    //Configure sliders.
    mTurningScroll->setScrollRange(1.0 / SLIDER_QUANTUM);
    mUpDownScroll->setScrollRange(1.0 / SLIDER_QUANTUM);
    mTurningScroll->setMoveToClick(true);
    mUpDownScroll->setMoveToClick(true);
    mTurningScroll->setScrollPosition(GlbVar.settings.controls.turningSensitivity / SLIDER_QUANTUM);
    mUpDownScroll->setScrollPosition(GlbVar.settings.controls.upDownSensitivity / SLIDER_QUANTUM);

    //Center it.
    int winHeight = GlbVar.ogreWindow->getHeight();
    int winWidth = GlbVar.ogreWindow->getWidth();
    int height = mWindow->getHeight();
    int width = mWindow->getWidth();
    mWindow->setCoord(MyGUI::IntCoord((winWidth - width)*0.5, (winHeight - height)*0.5, width, height));

    //Set key setter callback.
    MyGUI::EnumeratorWidgetPtr iter = GlbVar.gui->findWidget<MyGUI::TabItem>("o_tab_controls")->getEnumerator();
    while (iter.next())
    {
        MyGUI::Widget *widget = iter.current();

        //If it's a key setter, give it the key setting callback.
        if (widget->getUserString("type") == "KeyChange") 
            widget->eventMouseButtonClick = MyGUI::newDelegate(this, &OptionsDialog::onClickChangeKey);
    }

    //OK! :P
    MyGUI::ButtonPtr button;
    button = GlbVar.gui->findWidget<MyGUI::Button>("o_but_ok");
    button->eventMouseButtonClick = MyGUI::newDelegate(this, &OptionsDialog::onClickOK);
}
//-------------------------------------------------------------------------------
OptionsDialog::~OptionsDialog()
{
    GlbVar.gui->destroyWidget(mWindow);
}
//-------------------------------------------------------------------------------
void OptionsDialog::tick()
{
    //Update key display.
    MyGUI::EnumeratorWidgetPtr iter = GlbVar.gui->findWidget<MyGUI::TabItem>("o_tab_controls")->getEnumerator();
    while (iter.next())
    {
        MyGUI::Widget *widget = iter.current();

        //If it's a key displayer, make it display the key it wants to display.
        if (widget->getUserString("type") == "KeyDisplay") 
            widget->setCaption(GlbVar.keyMap->keyToString(
                        GlbVar.settings.controls.keys[widget->getUserString("key")]
                        ));
    }

    //Tell user to press key if he has to.
    mPressKeyText->setVisible(mListeningForKey);
    if (mListeningForKey)
    {
        //Highlight the relevant key.
        GlbVar.gui->findWidget<MyGUI::StaticText>("o_c_txt_" + mCurrKey)->setTextColour(MyGUI::Colour(0.8,0.6,0.1));
    }

    //Get slider values and update settings.
    GlbVar.settings.controls.turningSensitivity = (mTurningScroll->getScrollPosition() + 1) * SLIDER_QUANTUM;
    GlbVar.settings.controls.upDownSensitivity = (mUpDownScroll->getScrollPosition() + 1) * SLIDER_QUANTUM;

    //Update slider text and checkbox. We update the sliders themselves only when we become visible.
    mTurningText->setCaption(Ogre::StringConverter::toString(GlbVar.settings.controls.turningSensitivity, 2));
    mUpDownText->setCaption(Ogre::StringConverter::toString(GlbVar.settings.controls.upDownSensitivity, 2));
    mInvertMouseCheckBox->setStateCheck(GlbVar.settings.controls.invertMouse);
}
//-------------------------------------------------------------------------------
void OptionsDialog::keyPressed(OIS::KeyCode kc)
{
    //If we're listening, then save the key.
    if (mListeningForKey)
    {
        GlbVar.settings.controls.keys[mCurrKey] = kc;
        stopListeningForKey();
    }
}
//-------------------------------------------------------------------------------
void OptionsDialog::stopListeningForKey()
{
    //Stop highlighting the key.
    GlbVar.gui->findWidget<MyGUI::StaticText>("o_c_txt_" + mCurrKey)->setTextColour(MyGUI::Colour(1,1,1));

    //Make button caption 'Change' again.
    GlbVar.gui->findWidget<MyGUI::Button>("o_c_but_" + mCurrKey)->setCaption("Change");

    mListeningForKey = false;
    mCurrKey = "";
}
//-------------------------------------------------------------------------------
void OptionsDialog::onClickChangeKey(MyGUI::WidgetPtr button)
{
    //If we're not already listening, we start doing so, otherwise we stop.
    if (!mListeningForKey)
    {
        mListeningForKey = true;
        mCurrKey = button->getUserString("key");
        
        //So that he knows where to click to cancel the key change.
        GlbVar.gui->findWidget<MyGUI::Button>("o_c_but_" + mCurrKey)->setCaption("Cancel");
    }
    else
    {
        stopListeningForKey();
    }
}
//-------------------------------------------------------------------------------
void OptionsDialog::onClickOK(MyGUI::WidgetPtr button)
{
    setVisible(false);
}
//-------------------------------------------------------------------------------
void OptionsDialog::onClickInvertMouse(MyGUI::WidgetPtr button)
{
    GlbVar.settings.controls.invertMouse = !GlbVar.settings.controls.invertMouse;
}
//-------------------------------------------------------------------------------
void OptionsDialog::updateDisplay()
{
}
//-------------------------------------------------------------------------------
