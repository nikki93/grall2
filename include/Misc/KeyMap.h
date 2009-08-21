/*
 * =====================================================================================
 *
 *       Filename:  KeyMap.h
 *
 *    Description:  OIS::KeyCode <-> Ogre::String conversions.
 *
 *        Created:  Friday 21 August 2009 11:25:28  IST
 *       Compiler:  gcc
 *
 *         Author:  Nikhilesh (nikki)
 *
 * =====================================================================================
 */

#ifndef __KEYMAP_H__
#define __KEYMAP_H__

class KeyMap
{
    protected:
        typedef std::map<OIS::KeyCode, Ogre::String> KM;
        KM mKeyMap;

    public:
        KeyMap()
        {
            mKeyMap[OIS::KC_UNASSIGNED] = "UNASSIGNED";
            mKeyMap[OIS::KC_ESCAPE] = "ESCAPE";
            mKeyMap[OIS::KC_1] = "1";
            mKeyMap[OIS::KC_2] = "2";
            mKeyMap[OIS::KC_3] = "3";
            mKeyMap[OIS::KC_4] = "4";
            mKeyMap[OIS::KC_5] = "5";
            mKeyMap[OIS::KC_6] = "6";
            mKeyMap[OIS::KC_7] = "7";
            mKeyMap[OIS::KC_8] = "8";
            mKeyMap[OIS::KC_9] = "9";
            mKeyMap[OIS::KC_0] = "0";
            mKeyMap[OIS::KC_MINUS] = "MINUS";
            mKeyMap[OIS::KC_EQUALS] = "EQUALS";
            mKeyMap[OIS::KC_BACK] = "BACK";
            mKeyMap[OIS::KC_TAB] = "TAB";
            mKeyMap[OIS::KC_Q] = "Q";
            mKeyMap[OIS::KC_W] = "W";
            mKeyMap[OIS::KC_E] = "E";
            mKeyMap[OIS::KC_R] = "R";
            mKeyMap[OIS::KC_T] = "T";
            mKeyMap[OIS::KC_Y] = "Y";
            mKeyMap[OIS::KC_U] = "U";
            mKeyMap[OIS::KC_I] = "I";
            mKeyMap[OIS::KC_O] = "O";
            mKeyMap[OIS::KC_P] = "P";
            mKeyMap[OIS::KC_LBRACKET] = "LBRACKET";
            mKeyMap[OIS::KC_RBRACKET] = "RBRACKET";
            mKeyMap[OIS::KC_RETURN] = "RETURN";
            mKeyMap[OIS::KC_LCONTROL] = "LCONTROL";
            mKeyMap[OIS::KC_A] = "A";
            mKeyMap[OIS::KC_S] = "S";
            mKeyMap[OIS::KC_D] = "D";
            mKeyMap[OIS::KC_F] = "F";
            mKeyMap[OIS::KC_G] = "G";
            mKeyMap[OIS::KC_H] = "H";
            mKeyMap[OIS::KC_J] = "J";
            mKeyMap[OIS::KC_K] = "K";
            mKeyMap[OIS::KC_L] = "L";
            mKeyMap[OIS::KC_SEMICOLON] = "SEMICOLON";
            mKeyMap[OIS::KC_APOSTROPHE] = "APOSTROPHE";
            mKeyMap[OIS::KC_GRAVE] = "GRAVE";
            mKeyMap[OIS::KC_LSHIFT] = "LSHIFT";
            mKeyMap[OIS::KC_BACKSLASH] = "BACKSLASH";
            mKeyMap[OIS::KC_Z] = "Z";
            mKeyMap[OIS::KC_X] = "X";
            mKeyMap[OIS::KC_C] = "C";
            mKeyMap[OIS::KC_V] = "V";
            mKeyMap[OIS::KC_B] = "B";
            mKeyMap[OIS::KC_N] = "N";
            mKeyMap[OIS::KC_M] = "M";
            mKeyMap[OIS::KC_COMMA] = "COMMA";
            mKeyMap[OIS::KC_PERIOD] = "PERIOD";
            mKeyMap[OIS::KC_SLASH] = "SLASH";
            mKeyMap[OIS::KC_RSHIFT] = "RSHIFT";
            mKeyMap[OIS::KC_MULTIPLY] = "MULTIPLY";
            mKeyMap[OIS::KC_LMENU] = "LMENU";
            mKeyMap[OIS::KC_SPACE] = "SPACE";
            mKeyMap[OIS::KC_CAPITAL] = "CAPITAL";
            mKeyMap[OIS::KC_F1] = "F1";
            mKeyMap[OIS::KC_F2] = "F2";
            mKeyMap[OIS::KC_F3] = "F3";
            mKeyMap[OIS::KC_F4] = "F4";
            mKeyMap[OIS::KC_F5] = "F5";
            mKeyMap[OIS::KC_F6] = "F6";
            mKeyMap[OIS::KC_F7] = "F7";
            mKeyMap[OIS::KC_F8] = "F8";
            mKeyMap[OIS::KC_F9] = "F9";
            mKeyMap[OIS::KC_F10] = "F10";
            mKeyMap[OIS::KC_NUMLOCK] = "NUMLOCK";
            mKeyMap[OIS::KC_SCROLL] = "SCROLL";
            mKeyMap[OIS::KC_NUMPAD7] = "NUMPAD7";
            mKeyMap[OIS::KC_NUMPAD8] = "NUMPAD8";
            mKeyMap[OIS::KC_NUMPAD9] = "NUMPAD9";
            mKeyMap[OIS::KC_SUBTRACT] = "SUBTRACT";
            mKeyMap[OIS::KC_NUMPAD4] = "NUMPAD4";
            mKeyMap[OIS::KC_NUMPAD5] = "NUMPAD5";
            mKeyMap[OIS::KC_NUMPAD6] = "NUMPAD6";
            mKeyMap[OIS::KC_ADD] = "ADD";
            mKeyMap[OIS::KC_NUMPAD1] = "NUMPAD1";
            mKeyMap[OIS::KC_NUMPAD2] = "NUMPAD2";
            mKeyMap[OIS::KC_NUMPAD3] = "NUMPAD3";
            mKeyMap[OIS::KC_NUMPAD0] = "NUMPAD0";
            mKeyMap[OIS::KC_DECIMAL] = "DECIMAL";
            mKeyMap[OIS::KC_OEM_102] = "OEM_102";
            mKeyMap[OIS::KC_F11] = "F11";
            mKeyMap[OIS::KC_F12] = "F12";
            mKeyMap[OIS::KC_F13] = "F13";
            mKeyMap[OIS::KC_F14] = "F14";
            mKeyMap[OIS::KC_F15] = "F15";
            mKeyMap[OIS::KC_KANA] = "KANA";
            mKeyMap[OIS::KC_ABNT_C1] = "ABNT_C1";
            mKeyMap[OIS::KC_CONVERT] = "CONVERT";
            mKeyMap[OIS::KC_NOCONVERT] = "NOCONVERT";
            mKeyMap[OIS::KC_YEN] = "YEN";
            mKeyMap[OIS::KC_ABNT_C2] = "ABNT_C2";
            mKeyMap[OIS::KC_NUMPADEQUALS] = "NUMPADEQUALS";
            mKeyMap[OIS::KC_PREVTRACK] = "PREVTRACK";
            mKeyMap[OIS::KC_AT] = "AT";
            mKeyMap[OIS::KC_COLON] = "COLON";
            mKeyMap[OIS::KC_UNDERLINE] = "UNDERLINE";
            mKeyMap[OIS::KC_KANJI] = "KANJI";
            mKeyMap[OIS::KC_STOP] = "STOP";
            mKeyMap[OIS::KC_AX] = "AX";
            mKeyMap[OIS::KC_UNLABELED] = "UNLABELED";
            mKeyMap[OIS::KC_NEXTTRACK] = "NEXTTRACK";
            mKeyMap[OIS::KC_NUMPADENTER] = "NUMPADENTER";
            mKeyMap[OIS::KC_RCONTROL] = "RCONTROL";
            mKeyMap[OIS::KC_MUTE] = "MUTE";
            mKeyMap[OIS::KC_CALCULATOR] = "CALCULATOR";
            mKeyMap[OIS::KC_PLAYPAUSE] = "PLAYPAUSE";
            mKeyMap[OIS::KC_MEDIASTOP] = "MEDIASTOP";
            mKeyMap[OIS::KC_VOLUMEDOWN] = "VOLUMEDOWN";
            mKeyMap[OIS::KC_VOLUMEUP] = "VOLUMEUP";
            mKeyMap[OIS::KC_WEBHOME] = "WEBHOME";
            mKeyMap[OIS::KC_NUMPADCOMMA] = "NUMPADCOMMA";
            mKeyMap[OIS::KC_DIVIDE] = "DIVIDE";
            mKeyMap[OIS::KC_SYSRQ] = "SYSRQ";
            mKeyMap[OIS::KC_RMENU] = "RMENU";
            mKeyMap[OIS::KC_PAUSE] = "PAUSE";
            mKeyMap[OIS::KC_HOME] = "HOME";
            mKeyMap[OIS::KC_UP] = "UP";
            mKeyMap[OIS::KC_PGUP] = "PGUP";
            mKeyMap[OIS::KC_LEFT] = "LEFT";
            mKeyMap[OIS::KC_RIGHT] = "RIGHT";
            mKeyMap[OIS::KC_END] = "END";
            mKeyMap[OIS::KC_DOWN] = "DOWN";
            mKeyMap[OIS::KC_PGDOWN] = "PGDOWN";
            mKeyMap[OIS::KC_INSERT] = "INSERT";
            mKeyMap[OIS::KC_DELETE] = "DELETE";
            mKeyMap[OIS::KC_LWIN] = "LWIN";
            mKeyMap[OIS::KC_RWIN] = "RWIN";
            mKeyMap[OIS::KC_APPS] = "APPS";
            mKeyMap[OIS::KC_POWER] = "POWER";
            mKeyMap[OIS::KC_SLEEP] = "SLEEP";
            mKeyMap[OIS::KC_WAKE] = "WAKE";
            mKeyMap[OIS::KC_WEBSEARCH] = "WEBSEARCH";
            mKeyMap[OIS::KC_WEBFAVORITES] = "WEBFAVORITES";
            mKeyMap[OIS::KC_WEBREFRESH] = "WEBREFRESH";
            mKeyMap[OIS::KC_WEBSTOP] = "WEBSTOP";
            mKeyMap[OIS::KC_WEBFORWARD] = "WEBFORWARD";
            mKeyMap[OIS::KC_WEBBACK] = "WEBBACK";
            mKeyMap[OIS::KC_MYCOMPUTER] = "MYCOMPUTER";
            mKeyMap[OIS::KC_MAIL] = "MAIL";
            mKeyMap[OIS::KC_MEDIASELECT] = "MEDIASELECT";
        }

        Ogre::String keyToString(OIS::KeyCode key)
        {
            return mKeyMap[key];
        }
        OIS::KeyCode stringToKey(Ogre::String key, OIS::KeyCode defaultKey)
        {
            KM::iterator iter = mKeyMap.begin();
            for (; iter != mKeyMap.end() && iter->second != key; ++iter);

            if (iter != mKeyMap.end())
                return iter->first;
            else
                return defaultKey;
        }
};

#endif
