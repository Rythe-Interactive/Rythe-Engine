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



function scandir(directory,recursive, extensions)
    directory = directory or ''
    recursive = recursive or false
    -- if string.sub(directory, -1) ~= '/' then directory = directory .. '/' end

    local currentDirectory = directory
    local fileList = {}
    local command = "dir " .. directory .. " /b"
    if recursive then command = command .. '/s' end

    for fileName in io.popen(command):lines() do
        if string.find(fileName,"include") then
            goto continue
        end
        if string.sub(fileName, -1) == '/' then
            -- Directory, don't do anything
        elseif string.sub(fileName, -1) == ':' then
            currentDirectory = string.sub(fileName, 1, -2) .. 'wat'
            -- if currentDirectory ~= directory then
                currentDirectory = currentDirectory .. '/'
            -- end
        elseif string.len(fileName) == 0 then
            -- Blank line
            currentDirectory = directory
        -- elseif string.find(fileName,"%.lua$") then
            -- File is a .lua file
        else
            if type(extensions) == 'table' then
                for _, extension in ipairs(extensions) do
                    if string.find(fileName,"%." .. extension .. "$") then
                        table.insert(fileList, currentDirectory .. fileName)
                    end
                end
            else
                table.insert(fileList, currentDirectory .. fileName)
            end
        end
        ::continue::
    end
    return fileList
end
cleanExts = {"vcxproj","vcxproj.filters","vcxproj.user"}

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

newaction
{
    trigger = "clean",
    description = "clean the project files",
    execute = function ()
        for key, value in ipairs(scandir("",true,cleanExts)) do
            os.remove(value)
        end
    end
}