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

#define SETTINGS_FILE (USER_PREFIX "Settings.ini")

void loadSettings()
{
    Ogre::ConfigFile cfg;

    try
    {
        cfg.loadDirect(SETTINGS_FILE);
    }
    catch (Ogre::FileNotFoundException &)
    {
        std::ofstream out(SETTINGS_FILE);
        out << std::endl;
        out.close();

        cfg.loadDirect(SETTINGS_FILE);
    }

    //--- Ogre -------------------------------------------------------------------------
    
    GlbVar.settings.ogre.pluginDirectory = cfg.getSetting("pluginDirectory", "ogre", ".");
    GlbVar.settings.ogre.plugins = Ogre::StringConverter::parseStringVector(
            cfg.getSetting("plugins", "ogre", "Plugin_CgProgramManager Plugin_ParticleFX Plugin_OctreeSceneManager")
            );

    GlbVar.settings.ogre.renderer = cfg.getSetting("renderer", "ogre", "OpenGL") == "Direct3D"
        ? Globals::Settings::OgreSettings::DIRECT3D
        : Globals::Settings::OgreSettings::OPENGL;

    GlbVar.settings.ogre.winWidth = Ogre::StringConverter::parseReal(cfg.getSetting("width", "ogre", "1024"));
    GlbVar.settings.ogre.winHeight = Ogre::StringConverter::parseReal(cfg.getSetting("height", "ogre", "768"));
    GlbVar.settings.ogre.winFullscreen = Ogre::StringConverter::parseBool(cfg.getSetting("fullscreen", "ogre", "no"));
    GlbVar.settings.ogre.FSAA = cfg.getSetting("FSAA", "ogre", "0");
    GlbVar.settings.ogre.vsync = cfg.getSetting("vsync", "ogre", "no");

    //--- Graphics ---------------------------------------------------------------------

    GlbVar.settings.graphics.lighting = Ogre::StringConverter::parseBool(cfg.getSetting("lighting", "graphics", "yes"));

    //--- Controls ---------------------------------------------------------------------

    GlbVar.settings.controls.turningSensitivity = Ogre::StringConverter::parseReal(cfg.getSetting("turningSensitivity", "controls", "0.2"));
    GlbVar.settings.controls.upDownSensitivity = Ogre::StringConverter::parseReal(cfg.getSetting("upDownSensitivity", "controls", "1"));
    GlbVar.settings.controls.invertMouse = Ogre::StringConverter::parseBool(cfg.getSetting("invertMouse", "controls", "1"));

    GlbVar.settings.controls.keys["forward"] = GlbVar.keyMap->stringToKey(cfg.getSetting("forward", "controls", "W"), OIS::KC_W);
    GlbVar.settings.controls.keys["backward"] = GlbVar.keyMap->stringToKey(cfg.getSetting("backward", "controls", "S"), OIS::KC_W);
    GlbVar.settings.controls.keys["left"] = GlbVar.keyMap->stringToKey(cfg.getSetting("left", "controls", "A"), OIS::KC_W);
    GlbVar.settings.controls.keys["right"] = GlbVar.keyMap->stringToKey(cfg.getSetting("right", "controls", "D"), OIS::KC_W);

    GlbVar.settings.controls.keys["dimensionSwitch"] = GlbVar.keyMap->stringToKey(cfg.getSetting("dimensionSwitch", "controls", "SPACE"), OIS::KC_W);
    GlbVar.settings.controls.keys["selfDestruct"] = GlbVar.keyMap->stringToKey(cfg.getSetting("selfDestruct", "controls", "Z"), OIS::KC_W);

    GlbVar.settings.controls.keys["peepLeft"] = GlbVar.keyMap->stringToKey(cfg.getSetting("peepLeft", "controls", "Q"), OIS::KC_W);
    GlbVar.settings.controls.keys["peepRight"] = GlbVar.keyMap->stringToKey(cfg.getSetting("peepRight", "controls", "E"), OIS::KC_W);

    //--- Misc. ------------------------------------------------------------------------

    GlbVar.settings.misc.fixCameraObstruction = Ogre::StringConverter::parseBool(cfg.getSetting("fixCameraObstruction", "misc", "yes"));

    //----------------------------------------------------------------------------------
}

void saveSettings()
{
    std::ofstream cfg(SETTINGS_FILE);

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

    cfg << "forward = " << GlbVar.keyMap->keyToString(GlbVar.settings.controls.keys["forward"]) << std::endl;
    cfg << "backward = " << GlbVar.keyMap->keyToString(GlbVar.settings.controls.keys["backward"]) << std::endl;
    cfg << "left = " << GlbVar.keyMap->keyToString(GlbVar.settings.controls.keys["left"]) << std::endl;
    cfg << "right = " << GlbVar.keyMap->keyToString(GlbVar.settings.controls.keys["right"]) << std::endl;
    cfg << "dimensionSwitch = " << GlbVar.keyMap->keyToString(GlbVar.settings.controls.keys["dimensionSwitch"]) << std::endl;
    cfg << "selfDestruct = " << GlbVar.keyMap->keyToString(GlbVar.settings.controls.keys["selfDestruct"]) << std::endl;
    cfg << "peepLeft = " << GlbVar.keyMap->keyToString(GlbVar.settings.controls.keys["peepLeft"]) << std::endl;
    cfg << "peepRight = " << GlbVar.keyMap->keyToString(GlbVar.settings.controls.keys["peepRight"]) << std::endl;
    cfg << std::endl;

    //--- Misc. ------------------------------------------------------------------------

    cfg << "[misc]" << std::endl;

    cfg << "fixCameraObstruction = " << GlbVar.settings.misc.fixCameraObstruction << std::endl;
    cfg << std::endl;
    
    //----------------------------------------------------------------------------------

    cfg.close();
}

