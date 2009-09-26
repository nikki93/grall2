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

package.postbuildcommands = { "rm `find -name '*.d'`" }

-- Search paths -----------------------------------------------------------------------------

package.includepaths = {
    "../../Libraries/ogre/btogre/include",                                   -- BtOgre
    "../../Libraries/ogre/ngf/include",                                      -- NGF

    "./include"                                                              -- GraLL2 files
}

package.libpaths = {
}

-- Libraries --------------------------------------------------------------------------------

package.links = {
    "python2.6",
    "boost_python",
    "boost_serialization",
    "BulletDynamics",
    "BulletCollision",
    "LinearMath"
}

-- pkg-configable stuff ---------------------------------------------------------------------

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

-- Files ------------------------------------------------------------------------------------

package.files = {
    matchrecursive("*.h", "*.cpp"),
}

-- Debug configuration ----------------------------------------------------------------------

debug.defines = { "DEBUG", "_DEBUG" }
debug.objdir = "obj/debug"
debug.target = "debug/" .. package.name .. "_d"

debug.buildoptions = { "-g" --[["-pg"]] }                                    -- '-pg' for gprof
-- debug.linkoptions = { "-pg" }                                             -- '-pg' for gprof

-- Release configuration --------------------------------------------------------------------

release.objdir = "obj/release"
release.target = "release/" .. package.name

release.buildoptions = { "-O3", "-funroll-loops" }

end

---------------------------------------------------------------------------------------------
-- Windows ----------------------------------------------------------------------------------
---------------------------------------------------------------------------------------------

if (windows) then

table.insert(package.defines, "WIN32") -- To fix a problem on Windows.

-- Search paths -----------------------------------------------------------------------------

package.includepaths = {
    "./include"                                                              -- GraLL2 files
}

packagepaths = {
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

package.files = {
    matchrecursive("*.h", "*.cpp"),

    "..\\..\\Libraries\\ogre\\btogre\\BtOgre.cpp",
    "..\\..\\Libraries\\ogre\\ngf\\Ngf.cpp",
    "..\\..\\Libraries\\ogre\\ngf\\plugins\\ngfbullet\\NgfBullet.cpp",
    "..\\..\\Libraries\\ogre\\ngf\\plugins\\ngfpython\\NgfPython.cpp",
    "..\\..\\Libraries\\ogre\\ngf\\plugins\\ngfserialisation\\NgfSerialisation.cpp"
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
