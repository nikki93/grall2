---------------------------------------------------------------------------------------------
-------------------------------- GraLL2 'Premake.lua' file ----------------------------------
---------------------------------------------------------------------------------------------

-- Project ----------------------------------------------------------------------------------

project.name = "GraLL2"
project.bindir = "bin"

-- Package ----------------------------------------------------------------------------------

package = newpackage()

package.name = "GraLL2"
package.kind = "exe"
package.language = "c++"
package.configs = { "Debug", "Release" }

debug = package.config["Debug"]
release = package.config["Release"]

---------------------------------------------------------------------------------------------
-- Linux ------------------------------------------------------------------------------------
---------------------------------------------------------------------------------------------

if (linux) then

package.postbuildcommands = { 
    "rm `find -name '*.d'`"
}

-- pkg-config -------------------------------------------------------------------------------

package.buildoptions = { "`pkg-config --cflags OGRE MYGUI bullet OgreAL OIS`" }
package.linkoptions = { "`pkg-config --libs OGRE MYGUI bullet OgreAL OIS`" }

-- Search paths -----------------------------------------------------------------------------

package.includepaths = {
    "../btogre/include",                                                     -- BtOgre
    "../ngf/include",                                                        -- NGF
    "/usr/include/python2.6",                                                -- Python


    "./ext",                                                                 -- 'External' stuff
    "./include"                                                              -- GraLL2 files
}

package.libpaths = {
}

-- Libraries --------------------------------------------------------------------------------

package.links = {
    "MyGUI.OgrePlatform",
    "boost_python",
    "boost_serialization",
    "python2.6",
}

-- Files ------------------------------------------------------------------------------------

-- 'External' files are symlinked.
package.files = {
    matchrecursive("*.h", "*.cpp"),
}

-- Debug configuration ----------------------------------------------------------------------

debug.defines = { "DEBUG", "_DEBUG" }
debug.objdir = "obj/debug"
debug.target = "debug/" .. package.name .. "_d"

debug.buildoptions = { "-g" ,"-pg" }
-- debug.buildoptions = { "-g", "-pg", "-E" }  -- Preprocessor only.
debug.linkoptions = { "-pg" }

-- Release configuration --------------------------------------------------------------------

release.objdir = "obj/release"
release.target = "release/" .. package.name

release.buildoptions = { "-O3", "-funroll-loops" }

end

---------------------------------------------------------------------------------------------
-- Windows ----------------------------------------------------------------------------------
---------------------------------------------------------------------------------------------

if (windows) then

table.insert(package.defines, "WIN32")      -- To fix a problem on Windows
package.buildoptions = { "/MP" }            -- Parallel building (good for multicore systems)

-- Search paths -----------------------------------------------------------------------------

package.includepaths = {
    "../../Libraries/boost",                                                 -- Boost
    "../../Libraries/btogre/include",                                        -- BtOgre
    "../../Libraries/bullet/include",                                        -- Bullet
    "../../Libraries/mygui/MyGUIEngine/include",                             -- MyGUI
    "../../Libraries/ngf/include",                                           -- NGF
    "../../Libraries/ogg/include",                                           -- Ogg
    "../../Libraries/ogre/Dependencies/include",                             -- OIS
    "../../Libraries/ogre/OgreMain/include",                                 -- Ogre
    "../../Libraries/ogreal/include",                                        -- OgreAL
    "../../Libraries/openal/include",                                        -- OpenAL
    "../../Libraries/python/include",                                        -- Python
    "../../Libraries/vorbis/include",                                        -- Vorbis

    "./ext",                                                                 -- 'External' stuff
    "./include"                                                              -- GraLL2 files
}

package.libpaths = {
    "../../Libraries/boost/lib",                                             -- Boost
    "../../Libraries/bullet/lib/$(ConfigurationName)",                       -- Bullet
    "../../Libraries/mygui/MyGUIEngine/lib/$(ConfigurationName)",            -- MyGUI
    "../../Libraries/ogre/Dependencies/lib/$(ConfigurationName)",            -- OIS
    "../../Libraries/ogre/build/lib/$(ConfigurationName)",                   -- Ogre
    "../../Libraries/ogreal/lib/$(ConfigurationName)",                       -- OgreAL
    "../../Libraries/openal/libs/Win32",                                     -- OpenAL
    "../../Libraries/python/libs"                                            -- Python
}

-- Libraries --------------------------------------------------------------------------------

debug.links = {
    "bulletdynamics",
    "bulletcollision",
    "linearmath",
    "python26",
    "OIS_d",
    "OgreMain_d",
    "MyGUI_d",
    "OgreAL_d"
}

release.links = {
    "bulletdynamics",
    "bulletcollision",
    "linearmath",
    "python26",
    "OIS",
    "OgreMain",
    "MyGUI",
    "OgreAL"
}

-- Files ------------------------------------------------------------------------------------

-- 'External' files too, like Ngf.cpp etc.
package.files = {
    matchrecursive("*.h", "*.cpp"),

    "../../Libraries/btogre/BtOgre.cpp",
    "../../Libraries/ngf/Ngf.cpp",
    "../../Libraries/ngf/plugins/ngfbullet/NgfBullet.cpp",
    "../../Libraries/ngf/plugins/ngfpython/NgfPython.cpp",
    "../../Libraries/ngf/plugins/ngfserialisation/NgfSerialisation.cpp"
}

-- Debug configuration ----------------------------------------------------------------------

debug.defines = { "DEBUG", "_DEBUG" }
debug.objdir = "obj/debug"
debug.target = "debug/" .. package.name .. "_d"

debug.linkoptions = { "/NODEFAULTLIB:libcpmt.lib", "/NODEFAULTLIB:LIBCMT" }

-- Release configuration --------------------------------------------------------------------

release.objdir = "obj/release"
release.target = "release/" .. package.name

release.linkoptions = { "/NODEFAULTLIB:libcpmt.lib", "/NODEFAULTLIB:LIBCMT" }

end
