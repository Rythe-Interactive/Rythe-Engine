--! Args Central Build Script for Args-Engine
--[[
author: Raphael Baier
copyright: (c) 2020 Raphael Baier, The Args-Team

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
workspace "args"
    configurations { "Debug32", "Release32", "Debug64", "Release64" }

-- core module, must not have any dependencies and should be first
include "core/build-core.lua"

project "*"
    includedirs { "../deps/include/" }

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
