--! Legion Central Build Script for Legion-Engine
--[[
authors: Raphael Baier, Glyn Leine
copyright: (c) 2020 Raphael Baier, The Legion-Team

Permission is hereby granted, free of charge, to any person obtaining a copy of this
software and associated documentation files (the "Software"), to deal in the Software
without restriction, including without limitation the rights to use, copy, modify, merge,
publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons
to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or
substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE
FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
DEALINGS IN THE SOFTWARE.

]]--


--[[ //NOTE:(algo-ryth-mix)
        The License of this module is problematic. Or rather the lack thereof
        I have contacted the maintainers of this repository, to maybe get them
        to add a license, I am still waiting however

        for the time being the module int <root>/tools/export-compile-commands will
        stay as a simple file-ptr to avoid problems

        Why is this needed?
        this can generate compile-commands for us, which clang-* can use to
        analyze our code

require "../tools/export-compile-commands"
]]--

-- root workspace, all sub-project should be included
workspace "legion-engine"
    configurations { "Debug64", "Release64" }

-- core module, must not have any dependencies and should be first
include "legion/engine/core/build-core.lua"
--include "legion/engine/physics/build-physics.lua"
--include "legion/engine/scripting/build-scripting.lua"
--include "legion/engine/networking/build-networking.lua"
--include "legion/engine/audio/build-audio.lua"
--include "legion/engine/application/build-application.lua"
--include "legion/engine/rendering/build-rendering.lua"
include "applications/unit_tests/build-tests.lua"

project "*"
    includedirs { "deps/include/" }
    targetdir "bin/%{cfg.buildcfg}"
    libdirs { "deps/lib/", "bin/%{cfg.buildcfg}/" }
    defines { "USE_OPTICK=0" }
        
    filter "configurations:Debug*"
        buildoptions { "-fsanitize=address,undefined" }
        linkoptions { "-fsanitize=address,undefined" }
        defines {"DEBUG"}
        symbols "On"

    filter "configurations:Release*"
        defines {"NDEBUG"}
        optimize "On"

    filter "configurations:*64"
        architecture "x86_64"

-- how to build:
--[[
    you require a copy of premake5 which can be obtained from https://premake.github.io/download.html#v5
    if you are on linux, here is a one liner to install it:

    wget -qO- https://github.com/premake/premake-core/releases/download/v5.0.0-alpha15/premake-5.0.0-alpha15-linux.tar.gz | tar xvz premake5 | xargs -I"{}" sudo mv {} /usr/bin

    on windows make sure that premake5 is in your path

    then invoke premake
        on windows: premake5 vs2019
        on linux: premake5 gmake
        to build the compilation database:
        premake5 export-compile-commands (on any platform with export-compile-commands installed, see note above)

]]--
