--! Rythe Central Build Script for Rythe-Engine
--[[
authors: Rowan Ramsey, Glyn Leine
copyright: (c) 2023 The Rythe-Team

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


function formatEngineModulePath(moduleName)
    return string.format("rythe/engine/%s/build-%s.lua", moduleName, moduleName, moduleName)
end

function formatApplicationPath(moduleName)
    return string.format("applications/%s/build-%s.lua", moduleName, moduleName)
end

function formatExternalProject(projectName)
    return string.format("rythe/engine/core/third_party/%s/build-%s.lua",projectName,projectName)
end

function createProject(groupName,projectName,kindName)
    print("Building " .. projectName)
    group ("" .. groupName)
    project ("" .. projectName)
        kind (""..kindName)
        location ("src/"..projectName)
        architecture "x64"
        toolset "clang"
        language "C++"
        cppdialect "C++20"
        targetdir "$(SolutionDir)bin\\lib"
        libdirs {"$(SolutionDir)bin\\lib\\"}
        objdir "$(SolutionDir)bin\\obj"
        defines {"RYTHE_INTERNAL", "PROJECT_NAME="..projectName}
        filter "configurations:Debug*"
            defines {"DEBUG"}
            symbols "On"
            targetsuffix "-d"
        filter "configurations:Release*"
            defines {"NDEBUG"}
            optimize "On"

        filter {}
    group ""
end

-- root workspace, all sub-project should be included
workspace "rythe"
    configurations { "Debug64", "Release64" }

-- core module, must not have any dependencies and should be first
include(formatEngineModulePath("core"))
include(formatEngineModulePath("application"))
include(formatEngineModulePath("graphics"))
include(formatEngineModulePath("physics"))
include(formatEngineModulePath("audio"))
include(formatExternalProject("rythe-standard-library"))

include(formatApplicationPath("sandbox"))
include(formatApplicationPath("rsl_test"))

os.chdir(_WORKING_DIR)

local r = require("premake/rythe")

r.test()

newaction
{
    trigger = "clean",
    description = "clean the project files",
    execute = function ()
        local ok, err = os.remove{"**.vcxproj", "**.vcxproj.filters"}
        if not ok then
            error(err)
        end
    end
}