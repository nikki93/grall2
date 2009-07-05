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
package.configs = { "Debug", "Release" } -- If you add more, configure them at the bottom.

package.postbuildcommands = { "rm `find -name '*.d'`" }

if (windows) then
   table.insert(package.defines, "WIN32") -- To fix a problem on Windows.
end

-- Search paths. Any libaries without pkg-configs need to be listed here --------------------

package.includepaths = {
    -- Absolute include directories.
    -- "<boostdir>",                                                         -- Boost
    "/home/nikki/Development/Libraries/ogre/btogre/include",                 -- BtOgre
    "/home/nikki/Development/Libraries/ogre/ngf/include",                    -- NGF

    -- Relative include directories.
    "./include"                                                              -- GraLL2 files
}

package.libpaths = {
    -- Library directories.
}

-- Libraries to link to ---------------------------------------------------------------------

package.links = {
    -- Libaries.
    "python2.6",
    "boost_python",
    "boost_serialization",
    "BulletDynamics",
    "BulletCollision",
    "LinearMath"
}

-- pkg-configable stuff ---------------------------------------------------------------------

if (linux) then
    package.buildoptions = {
	"`pkg-config OGRE --cflags`" ..
	"`pkg-config OIS --cflags`" ..
	"`pkg-config MyGUI --cflags`" ..
	"`pkg-config OgreAL --cflags`"
    }

    package.linkoptions = {
	"`pkg-config OGRE --libs`" ..
	"`pkg-config OIS --libs`" ..
	"`pkg-config MyGUI --libs`" ..
	"`pkg-config OgreAL --libs`"
    }
end

-- Files ------------------------------------------------------------------------------------

-- Also add library .cpp files (Ngf.cpp, BtOgre.cpp etc.). I actually have symlinks to them
-- but they're not on git (they're listed in .git/info/exclude, that's why).
package.pchheader = "include/Precomp.h"
package.files = {
    matchrecursive("*.h", "*.cpp"),
}

-- Debug configuration ----------------------------------------------------------------------

debug = package.config["Debug"]
debug.defines = { "DEBUG", "_DEBUG" }
debug.objdir = "obj/debug"
debug.target = "debug/" .. package.name .. "_d"

debug.buildoptions = { "-g" --[["-pg"]] }                                    -- '-pg' for gprof
-- debug.linkoptions = { "-pg" }                                             -- '-pg' for gprof

-- Release configuration --------------------------------------------------------------------

release = package.config["Release"]
release.objdir = "obj/release"
release.target = "release/" .. package.name

release.buildoptions = { "-O3", "-funroll-loops" }
