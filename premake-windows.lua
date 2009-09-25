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

if (windows) then
   table.insert(package.defines, "WIN32") -- To fix a problem on Windows.
end

-- Search paths. Any libaries without pkg-configs need to be listed here --------------------

package.includepaths = {
    -- Absolute include directories.
    -- Relative include directories.
    "./include"                                                              -- GraLL2 files
}

packagepaths = {
    -- Library directories.
}

-- Files ------------------------------------------------------------------------------------

-- Also add library .cpp files (Ngf.cpp, BtOgre.cpp etc.). I actually have symlinks to them
-- but they're not on git (they're listed in .git/info/exclude, that's why).
-- package.pchheader = "include/Precomp.h"
package.files = {
    matchrecursive("*.h", "*.cpp"),
}

-- Debug configuration ----------------------------------------------------------------------

debug = package.config["Debug"]
debug.defines = { "DEBUG", "_DEBUG" }
debug.objdir = "obj/debug"
debug.target = "debug/" .. package.name .. "_d"

-- debug.buildoptions = { "-g" --[["-pg"]] }                                 -- '-pg' for gprof
-- debug.linkoptions = { "-pg" }                                             -- '-pg' for gprof

-- Release configuration --------------------------------------------------------------------

release = package.config["Release"]
release.objdir = "obj/release"
release.target = "release/" .. package.name

-- release.buildoptions = { "-O3", "-funroll-loops" }

-- Libraries to link to ---------------------------------------------------------------------

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
