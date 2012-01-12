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
    "rm -f `find -name '*.d'`",
    "scripts/compilePython.sh"
}

if (options["data_prefix"]) then
    table.insert(package.buildoptions, "-D 'DATA_PREFIX=\"" .. options["data_prefix"] .. "\"'")
end
if (options["use_home"]) then
    table.insert(package.buildoptions, "-D USE_HOME")
end

-- pkg-config -------------------------------------------------------------------------------

table.insert(package.buildoptions, "`pkg-config --cflags OGRE MYGUI bullet OgreAL OIS`")
package.linkoptions = { "`pkg-config --libs OGRE MYGUI bullet OgreAL OIS`" }

-- Search paths -----------------------------------------------------------------------------

package.includepaths = {
    "/usr/include/python2.7",                                                -- Python
    "./include",                                                             -- GraLL2 files
    "./lib"                                                                  -- NGF, BtOgre
}

package.libpaths = {
}

-- Libraries --------------------------------------------------------------------------------

package.links = {
    "MyGUI.OgrePlatform",
    "boost_python",
    "boost_filesystem",
    "boost_system",
    "boost_serialization",
    "python2.7"
}

-- Files ------------------------------------------------------------------------------------

package.files = {
    matchrecursive("*.h", "*.cpp"),
}

-- Debug configuration ----------------------------------------------------------------------

table.insert(debug.defines, "DEBUG")
table.insert(debug.defines, "_DEBUG")
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
    "../../Libraries/bullet/include",                                        -- Bullet
    "../../Libraries/mygui/MyGUIEngine/include",                             -- MyGUI
    "../../Libraries/mygui/Platforms/Ogre/OgrePlatform/include",             -- MyGUI.OgrePlatform
    "../../Libraries/ogg/include",                                           -- Ogg
    "../../Libraries/ogre/Dependencies/include",                             -- OIS
    "../../Libraries/ogre/OgreMain/include",                                 -- Ogre
    "../../Libraries/ogre/build/include",                                    -- Ogre build
    "../../Libraries/ogreal/include",                                        -- OgreAL
    "../../Libraries/openal/include",                                        -- OpenAL
    "../../Libraries/python/include",                                        -- Python
    "../../Libraries/python/PC",                                             -- Python (Windows config)
    "../../Libraries/vorbis/include",                                        -- Vorbis

    "./include",                                                             -- GraLL2 files
    "./lib"                                                                  -- NGF, BtOgre
}

package.libpaths = {
    "../../Libraries/boost/stage/lib",                                       -- Boost
    "../../Libraries/bullet/lib/$(ConfigurationName)",                       -- Bullet
    "../../Libraries/mygui/build/lib/$(ConfigurationName)",                  -- MyGUI
    "../../Libraries/ogre/Dependencies/lib/$(ConfigurationName)",            -- OIS
    "../../Libraries/ogre/build/lib/$(ConfigurationName)",                   -- Ogre
    "../../Libraries/ogreal/lib/$(ConfigurationName)",                       -- OgreAL
    "../../Libraries/openal/libs/Win32",                                     -- OpenAL
    "../../Libraries/python/PCBuild"                                         -- Python
}

-- Libraries --------------------------------------------------------------------------------

release.links = {
    "bulletdynamics",
    "bulletcollision",
    "linearmath",
    "python27",
    "OIS",
    "OgreMain",
    "MyGUIEngine",
    "MyGUI.OgrePlatform",
    "OgreAL"
}

debug.links = {
    "bulletdynamics",
    "bulletcollision",
    "linearmath",
    "python27_d",
    "OIS_d",
    "OgreMain_d",
    "MyGUIEngine_d",
    "MyGUI.OgrePlatform_d",
    "OgreAL_d"
}

-- Files ------------------------------------------------------------------------------------

package.files = {
    matchrecursive("*.h", "*.cpp"),
}

-- Debug configuration ----------------------------------------------------------------------

debug.defines = { "DEBUG", "_DEBUG" }
debug.objdir = "obj/debug"
debug.target = "debug/" .. package.name .. "_d"

debug.linkoptions = { "/NODEFAULTLIB:libcpmt.lib", "/NODEFAULTLIB:LIBCMT", "/SUBSYSTEM:WINDOWS" }

-- Release configuration --------------------------------------------------------------------

release.objdir = "obj/release"
release.target = "release/" .. package.name

release.linkoptions = { "/NODEFAULTLIB:libcpmt.lib", "/NODEFAULTLIB:LIBCMT", "/SUBSYSTEM:WINDOWS" }

end
