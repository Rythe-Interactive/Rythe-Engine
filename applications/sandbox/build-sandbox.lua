--! Legion Sandbox Build Script for premake5
--[[
author: Glyn Leine
copyright: (c) 2020 The Legion-Team

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

group "applications"
project "sandbox"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++20"
    targetdir "../../build/%{cfg.buildcfg}"
    includedirs { "../","../../rythe/engine/core/src/"}

    files { "**.h", "**.hpp" ,"**.inl","**.c", "**.cpp" }

    include "../../rythe/engine/core/src/core/include-core.lua"
    include "../../rythe/engine/application/src/application/include-application.lua"
    include "../../rythe/engine/graphics/src/graphics/include-graphics.lua"
group ""

       
