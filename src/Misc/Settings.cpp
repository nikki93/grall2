/*
 * =====================================================================================
 *
 *       Filename:  Settings.cpp
 *
 *    Description:  Saving/loading of settings from the config file.
 *
 *        Created:  06/26/2009 03:30:52 PM
 *       Compiler:  gcc
 *
 *         Author:  Nikhilesh (nikki)
 *
 * =====================================================================================
 */

#include "Globals.h"

#define SETTINGS_FILE (USER_PREFIX + "Settings.ini")

typedef std::map<OIS::KeyCode, Ogre::String> KeyMap;
static KeyMap getKeyMap();
static Ogre::String keyToString(OIS::KeyCode key);
static OIS::KeyCode stringToKey(Ogre::String key, OIS::KeyCode defaultKey = OIS::KC_UNASSIGNED);

void loadSettings()
{
    Ogre::ConfigFile cfg;

    try
    {
        cfg.loadDirect(SETTINGS_FILE);
    }
    catch (Ogre::FileNotFoundException &e)
    {
        std::ofstream out(SETTINGS_FILE.c_str());
        out << std::endl;
        out.close();

        cfg.loadDirect(SETTINGS_FILE);
    }

    //--- Ogre -------------------------------------------------------------------------
    
    GlbVar.settings.ogre.pluginDirectory = cfg.getSetting("pluginDirectory", "ogre", ".");
    GlbVar.settings.ogre.plugins = Ogre::StringConverter::parseStringVector(
            cfg.getSetting("plugins", "ogre", "Plugin_CgProgramManager.so Plugin_ParticleFX.so Plugin_OctreeSceneManager.so")
            );

    GlbVar.settings.ogre.renderer = cfg.getSetting("renderer", "ogre", "OpenGL") == "Direct3D"
        ? Globals::Settings::OgreSettings::DIRECT3D
        : Globals::Settings::OgreSettings::OPENGL;

    GlbVar.settings.ogre.winWidth = Ogre::StringConverter::parseReal(cfg.getSetting("width", "ogre", "1024"));
    GlbVar.settings.ogre.winHeight = Ogre::StringConverter::parseReal(cfg.getSetting("height", "ogre", "768"));
    GlbVar.settings.ogre.winFullscreen = Ogre::StringConverter::parseBool(cfg.getSetting("fullscreen", "ogre", "no"));
    GlbVar.settings.ogre.FSAA = cfg.getSetting("FSAA", "ogre", "0");
    GlbVar.settings.ogre.vsync = cfg.getSetting("vsync", "ogre", "no");

    //--- Ogre -------------------------------------------------------------------------

    GlbVar.settings.graphics.lighting = Ogre::StringConverter::parseBool(cfg.getSetting("lighting", "graphics", "yes"));

    //--- Controls ---------------------------------------------------------------------

    GlbVar.settings.controls.turningSensitivity = Ogre::StringConverter::parseReal(cfg.getSetting("turningSensitivity", "controls", "0.2"));
    GlbVar.settings.controls.upDownSensitivity = Ogre::StringConverter::parseReal(cfg.getSetting("upDownSensitivity", "controls", "1"));
    GlbVar.settings.controls.invertMouse = Ogre::StringConverter::parseBool(cfg.getSetting("invertMouse", "controls", "1"));

    GlbVar.settings.controls.forward = stringToKey(cfg.getSetting("forward", "controls", "W"), OIS::KC_W);
    GlbVar.settings.controls.backward = stringToKey(cfg.getSetting("backward", "controls", "S"), OIS::KC_W);
    GlbVar.settings.controls.left = stringToKey(cfg.getSetting("left", "controls", "A"), OIS::KC_W);
    GlbVar.settings.controls.right = stringToKey(cfg.getSetting("right", "controls", "D"), OIS::KC_W);

    GlbVar.settings.controls.dimensionSwitch = stringToKey(cfg.getSetting("dimensionSwitch", "controls", "SPACE"), OIS::KC_W);
    GlbVar.settings.controls.selfDestruct = stringToKey(cfg.getSetting("selfDestruct", "controls", "F10"), OIS::KC_W);

    GlbVar.settings.controls.peepLeft = stringToKey(cfg.getSetting("peepLeft", "controls", "Q"), OIS::KC_W);
    GlbVar.settings.controls.peepRight = stringToKey(cfg.getSetting("peepRight", "controls", "E"), OIS::KC_W);

    //----------------------------------------------------------------------------------
}

void saveSettings()
{
    std::ofstream cfg(SETTINGS_FILE.c_str());

    //--- Ogre -------------------------------------------------------------------------
    
    cfg << "[ogre]" << std::endl;

    cfg << "pluginDirectory = " << GlbVar.settings.ogre.pluginDirectory << std::endl;
    cfg << "plugins = ";
    for (Ogre::StringVector::iterator iter = GlbVar.settings.ogre.plugins.begin();
            iter != GlbVar.settings.ogre.plugins.end(); ++iter)
        cfg << (*iter) << " ";
    cfg << std::endl;
    cfg << "renderer = " << ((GlbVar.settings.ogre.renderer == Globals::Settings::OgreSettings::OPENGL) ? "OpenGL" : "Direct3D") 
        << std::endl;
    cfg << std::endl;

    cfg << "width = " << GlbVar.settings.ogre.winWidth << std::endl;
    cfg << "height = " << GlbVar.settings.ogre.winHeight << std::endl;
    cfg << "fullscreen = " << GlbVar.settings.ogre.winFullscreen << std::endl;
    cfg << "FSAA = " << GlbVar.settings.ogre.FSAA << std::endl;
    cfg << "vsync = " << GlbVar.settings.ogre.vsync << std::endl;
    cfg << std::endl;

    //--- Graphics ---------------------------------------------------------------------

    cfg << "[graphics]" << std::endl;

    cfg << "lighting = " << GlbVar.settings.graphics.lighting << std::endl;
    cfg << std::endl;
    
    //--- Controls ---------------------------------------------------------------------

    cfg << "[controls]" << std::endl;

    cfg << "turningSensitivity = " << GlbVar.settings.controls.turningSensitivity << std::endl;
    cfg << "upDownSensitivity = " << GlbVar.settings.controls.upDownSensitivity << std::endl;
    cfg << "invertMouse = " << GlbVar.settings.controls.invertMouse << std::endl;
    cfg << std::endl;

    cfg << "forward = " << keyToString(GlbVar.settings.controls.forward) << std::endl;
    cfg << "backward = " << keyToString(GlbVar.settings.controls.backward) << std::endl;
    cfg << "left = " << keyToString(GlbVar.settings.controls.left) << std::endl;
    cfg << "right = " << keyToString(GlbVar.settings.controls.right) << std::endl;
    cfg << "dimensionSwitch = " << keyToString(GlbVar.settings.controls.dimensionSwitch) << std::endl;
    cfg << "selfDestruct = " << keyToString(GlbVar.settings.controls.selfDestruct) << std::endl;
    cfg << "peepLeft = " << keyToString(GlbVar.settings.controls.peepLeft) << std::endl;
    cfg << "peepRight = " << keyToString(GlbVar.settings.controls.peepRight) << std::endl;
    cfg << std::endl;

    //----------------------------------------------------------------------------------

    cfg.close();

}

//--- Key conversion -------------------------------------------------------------------
static Ogre::String keyToString(OIS::KeyCode key)
{
    static KeyMap keyMap = getKeyMap();
    return keyMap[key];
}
static OIS::KeyCode stringToKey(Ogre::String key, OIS::KeyCode defaultKey)
{
    static KeyMap keyMap = getKeyMap();

    KeyMap::iterator iter = keyMap.begin();
    for (; iter->second != key; ++iter);

    if (iter != keyMap.end())
        return iter->first;
    else
        return defaultKey;
}
static KeyMap getKeyMap()
{
    KeyMap keyMap;
    keyMap[OIS::KC_UNASSIGNED] = "UNASSIGNED";
    keyMap[OIS::KC_ESCAPE] = "ESCAPE";
    keyMap[OIS::KC_1] = "1";
    keyMap[OIS::KC_2] = "2";
    keyMap[OIS::KC_3] = "3";
    keyMap[OIS::KC_4] = "4";
    keyMap[OIS::KC_5] = "5";
    keyMap[OIS::KC_6] = "6";
    keyMap[OIS::KC_7] = "7";
    keyMap[OIS::KC_8] = "8";
    keyMap[OIS::KC_9] = "9";
    keyMap[OIS::KC_0] = "0";
    keyMap[OIS::KC_MINUS] = "MINUS";
    keyMap[OIS::KC_EQUALS] = "EQUALS";
    keyMap[OIS::KC_BACK] = "BACK";
    keyMap[OIS::KC_TAB] = "TAB";
    keyMap[OIS::KC_Q] = "Q";
    keyMap[OIS::KC_W] = "W";
    keyMap[OIS::KC_E] = "E";
    keyMap[OIS::KC_R] = "R";
    keyMap[OIS::KC_T] = "T";
    keyMap[OIS::KC_Y] = "Y";
    keyMap[OIS::KC_U] = "U";
    keyMap[OIS::KC_I] = "I";
    keyMap[OIS::KC_O] = "O";
    keyMap[OIS::KC_P] = "P";
    keyMap[OIS::KC_LBRACKET] = "LBRACKET";
    keyMap[OIS::KC_RBRACKET] = "RBRACKET";
    keyMap[OIS::KC_RETURN] = "RETURN";
    keyMap[OIS::KC_LCONTROL] = "LCONTROL";
    keyMap[OIS::KC_A] = "A";
    keyMap[OIS::KC_S] = "S";
    keyMap[OIS::KC_D] = "D";
    keyMap[OIS::KC_F] = "F";
    keyMap[OIS::KC_G] = "G";
    keyMap[OIS::KC_H] = "H";
    keyMap[OIS::KC_J] = "J";
    keyMap[OIS::KC_K] = "K";
    keyMap[OIS::KC_L] = "L";
    keyMap[OIS::KC_SEMICOLON] = "SEMICOLON";
    keyMap[OIS::KC_APOSTROPHE] = "APOSTROPHE";
    keyMap[OIS::KC_GRAVE] = "GRAVE";
    keyMap[OIS::KC_LSHIFT] = "LSHIFT";
    keyMap[OIS::KC_BACKSLASH] = "BACKSLASH";
    keyMap[OIS::KC_Z] = "Z";
    keyMap[OIS::KC_X] = "X";
    keyMap[OIS::KC_C] = "C";
    keyMap[OIS::KC_V] = "V";
    keyMap[OIS::KC_B] = "B";
    keyMap[OIS::KC_N] = "N";
    keyMap[OIS::KC_M] = "M";
    keyMap[OIS::KC_COMMA] = "COMMA";
    keyMap[OIS::KC_PERIOD] = "PERIOD";
    keyMap[OIS::KC_SLASH] = "SLASH";
    keyMap[OIS::KC_RSHIFT] = "RSHIFT";
    keyMap[OIS::KC_MULTIPLY] = "MULTIPLY";
    keyMap[OIS::KC_LMENU] = "LMENU";
    keyMap[OIS::KC_SPACE] = "SPACE";
    keyMap[OIS::KC_CAPITAL] = "CAPITAL";
    keyMap[OIS::KC_F1] = "F1";
    keyMap[OIS::KC_F2] = "F2";
    keyMap[OIS::KC_F3] = "F3";
    keyMap[OIS::KC_F4] = "F4";
    keyMap[OIS::KC_F5] = "F5";
    keyMap[OIS::KC_F6] = "F6";
    keyMap[OIS::KC_F7] = "F7";
    keyMap[OIS::KC_F8] = "F8";
    keyMap[OIS::KC_F9] = "F9";
    keyMap[OIS::KC_F10] = "F10";
    keyMap[OIS::KC_NUMLOCK] = "NUMLOCK";
    keyMap[OIS::KC_SCROLL] = "SCROLL";
    keyMap[OIS::KC_NUMPAD7] = "NUMPAD7";
    keyMap[OIS::KC_NUMPAD8] = "NUMPAD8";
    keyMap[OIS::KC_NUMPAD9] = "NUMPAD9";
    keyMap[OIS::KC_SUBTRACT] = "SUBTRACT";
    keyMap[OIS::KC_NUMPAD4] = "NUMPAD4";
    keyMap[OIS::KC_NUMPAD5] = "NUMPAD5";
    keyMap[OIS::KC_NUMPAD6] = "NUMPAD6";
    keyMap[OIS::KC_ADD] = "ADD";
    keyMap[OIS::KC_NUMPAD1] = "NUMPAD1";
    keyMap[OIS::KC_NUMPAD2] = "NUMPAD2";
    keyMap[OIS::KC_NUMPAD3] = "NUMPAD3";
    keyMap[OIS::KC_NUMPAD0] = "NUMPAD0";
    keyMap[OIS::KC_DECIMAL] = "DECIMAL";
    keyMap[OIS::KC_OEM_102] = "OEM_102";
    keyMap[OIS::KC_F11] = "F11";
    keyMap[OIS::KC_F12] = "F12";
    keyMap[OIS::KC_F13] = "F13";
    keyMap[OIS::KC_F14] = "F14";
    keyMap[OIS::KC_F15] = "F15";
    keyMap[OIS::KC_KANA] = "KANA";
    keyMap[OIS::KC_ABNT_C1] = "ABNT_C1";
    keyMap[OIS::KC_CONVERT] = "CONVERT";
    keyMap[OIS::KC_NOCONVERT] = "NOCONVERT";
    keyMap[OIS::KC_YEN] = "YEN";
    keyMap[OIS::KC_ABNT_C2] = "ABNT_C2";
    keyMap[OIS::KC_NUMPADEQUALS] = "NUMPADEQUALS";
    keyMap[OIS::KC_PREVTRACK] = "PREVTRACK";
    keyMap[OIS::KC_AT] = "AT";
    keyMap[OIS::KC_COLON] = "COLON";
    keyMap[OIS::KC_UNDERLINE] = "UNDERLINE";
    keyMap[OIS::KC_KANJI] = "KANJI";
    keyMap[OIS::KC_STOP] = "STOP";
    keyMap[OIS::KC_AX] = "AX";
    keyMap[OIS::KC_UNLABELED] = "UNLABELED";
    keyMap[OIS::KC_NEXTTRACK] = "NEXTTRACK";
    keyMap[OIS::KC_NUMPADENTER] = "NUMPADENTER";
    keyMap[OIS::KC_RCONTROL] = "RCONTROL";
    keyMap[OIS::KC_MUTE] = "MUTE";
    keyMap[OIS::KC_CALCULATOR] = "CALCULATOR";
    keyMap[OIS::KC_PLAYPAUSE] = "PLAYPAUSE";
    keyMap[OIS::KC_MEDIASTOP] = "MEDIASTOP";
    keyMap[OIS::KC_VOLUMEDOWN] = "VOLUMEDOWN";
    keyMap[OIS::KC_VOLUMEUP] = "VOLUMEUP";
    keyMap[OIS::KC_WEBHOME] = "WEBHOME";
    keyMap[OIS::KC_NUMPADCOMMA] = "NUMPADCOMMA";
    keyMap[OIS::KC_DIVIDE] = "DIVIDE";
    keyMap[OIS::KC_SYSRQ] = "SYSRQ";
    keyMap[OIS::KC_RMENU] = "RMENU";
    keyMap[OIS::KC_PAUSE] = "PAUSE";
    keyMap[OIS::KC_HOME] = "HOME";
    keyMap[OIS::KC_UP] = "UP";
    keyMap[OIS::KC_PGUP] = "PGUP";
    keyMap[OIS::KC_LEFT] = "LEFT";
    keyMap[OIS::KC_RIGHT] = "RIGHT";
    keyMap[OIS::KC_END] = "END";
    keyMap[OIS::KC_DOWN] = "DOWN";
    keyMap[OIS::KC_PGDOWN] = "PGDOWN";
    keyMap[OIS::KC_INSERT] = "INSERT";
    keyMap[OIS::KC_DELETE] = "DELETE";
    keyMap[OIS::KC_LWIN] = "LWIN";
    keyMap[OIS::KC_RWIN] = "RWIN";
    keyMap[OIS::KC_APPS] = "APPS";
    keyMap[OIS::KC_POWER] = "POWER";
    keyMap[OIS::KC_SLEEP] = "SLEEP";
    keyMap[OIS::KC_WAKE] = "WAKE";
    keyMap[OIS::KC_WEBSEARCH] = "WEBSEARCH";
    keyMap[OIS::KC_WEBFAVORITES] = "WEBFAVORITES";
    keyMap[OIS::KC_WEBREFRESH] = "WEBREFRESH";
    keyMap[OIS::KC_WEBSTOP] = "WEBSTOP";
    keyMap[OIS::KC_WEBFORWARD] = "WEBFORWARD";
    keyMap[OIS::KC_WEBBACK] = "WEBBACK";
    keyMap[OIS::KC_MYCOMPUTER] = "MYCOMPUTER";
    keyMap[OIS::KC_MAIL] = "MAIL";
    keyMap[OIS::KC_MEDIASELECT] = "MEDIASELECT";

    return keyMap;
}
