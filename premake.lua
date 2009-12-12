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

-- Search paths -----------------------------------------------------------------------------

package.includepaths = {
    "../../Libraries/ogre/btogre/include",                                   -- BtOgre
    "../../Libraries/ogre/ngf/include",                                      -- NGF
    "/usr/local/include/OGRE",                                               -- Ogre
    "/usr/local/include/OIS",                                                -- OIS
    "/usr/local/include/MYGUI",                                              -- MyGUI
    "/usr/local/include/OgreAL",                                             -- OgreAL

    "./ext",                                                                 -- 'External' stuff
    "./include"                                                              -- GraLL2 files
}

package.libpaths = {
}

-- Libraries --------------------------------------------------------------------------------

package.links = {
    "BulletDynamics",
    "BulletCollision",
    "LinearMath",
    "OIS",
    "OgreAL",
    "OgreMain",
    "boost_python",
    "boost_serialization",
    "m",
    "mygui",
    "ogg",
    "openal",
    "pthread",
    "python2.6",
    "uuid",
    "vorbis",
    "vorbisfile"
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
    "../../Libraries/bullet/src",                                            -- Bullet
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
    "../../Libraries/bullet/out/$(ConfigurationName)8/libs",                 -- Bullet
    "../../Libraries/mygui/MyGUIEngine/lib/$(ConfigurationName)",            -- MyGUI
    "../../Libraries/ogre/Dependencies/lib/$(ConfigurationName)",            -- OIS
    "../../Libraries/ogre/build/lib/$(ConfigurationName)",                   -- Ogre
    "../../Libraries/ogreal/lib/$(ConfigurationName)",                       -- OgreAL
    "../../Libraries/openal/libs/Win32",                                     -- OpenAL
    "../../Libraries/python/libs"                                            -- Python
}

-- Libraries --------------------------------------------------------------------------------

debug.links = {
    "libbulletdynamics_d",
    "libbulletcollision_d",
    "libbulletmath_d",
    "python26",
    "OIS_d",
    "OgreMain_d",
    "MyGUI_d",
    "OgreAL_d"
}

release.links = {
    "libbulletdynamics",
    "libbulletcollision",
    "libbulletmath",
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
