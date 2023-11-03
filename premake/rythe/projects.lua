local fs = dofile("filesystem.lua")
local ctx = dofile("context.lua")
local utils = dofile("utils.lua")

local rythe = premake.rythe
local loadedProjects = rythe.loadedProjects
local buildSettings = rythe.buildSettings

local projects = {}

local function find(projectPath)
    local projectName = fs.fileName(projectPath)
    local group = fs.parentPath(projectPath)
    
    local projectFile = projectPath .. "/project.lua"

    if not os.isfile(projectFile) then
        projectFile = projectPath .. "/" .. projectName .. ".lua"

        if not os.isfile(projectFile) then
            return nil, group, projectName
        end
    end

    return projectFile, group, projectName
end

local function kindName(projectType, config)
    if projectType == "module" then
        return ctx.linkTarget()
    elseif projectType == "test" then
        return "ConsoleApp"
    elseif projectType == "application" then
        if config == rythe.Configuration.RELEASE then
            return "WindowedApp"
        else
            return "ConsoleApp"
        end
    elseif projectType == "static-libary" then
        return "SharedLib"
    elseif projectType == "header-only" then
        return "SharedItems"
    elseif projectType == "util" then
        return "Utility"
    end
    assert(false, "Unknown project type: \"" .. projectType .. "\"")
end

function projects.load(projectPath)
    print("Loading project at \"" .. projectPath .. "\"")

    local project = loadedProjects[projectPath]

    if project ~= nil then
        return project
    end

    local projectFile, group, name = find(projectPath)

    if projectFile == nil then
        print("Could not find project \"" .. group .. "/" .. name .. "\"")
        return nil
    end

    project = dofile(projectFile)

    if project == nil then
        print("Could not initialize project \"" .. group .. "/" .. name .. "\"")
        return nil
    end

    assert(project.group == group, "Group folder structure mismatch \"" .. group .. "\" vs \"" .. project.group .. "\"")
    assert(project.name == name, "Project name folder structure mismatch \"" .. name .. "\" vs \"" .. project.name .. "\"")
    assert(not utils.tableIsEmpty(project.types), "Project must hold an assembly type. (Use the type \"util\" if no source code is required)")
    if next(project.dependencies) == nil then
        project.dependencies = nil
    end

    if utils.tableIsEmpty(project.defines) then
        project.defines = { "PROJECT_NAME=" .. project.name }
    else
        project.defines = utils.concatTables(project.defines, { "PROJECT_NAME=" .. project.name })
    end

    project.src = projectFile
    project.location = projectPath

    loadedProjects[projectPath] = project

    return project
end

local function setupRelease()
    filter("configurations:Release")
    defines { "NDEBUG" }
    optimize("Full")
end

local function setupDevelopment()
    filter("configurations:Development")
    defines { "DEBUG" }
    optimize("Debug")
    inlining("Explicit")
    symbols("On")
end

local function setupDebug()
    filter("configurations:Debug")
    defines { "DEBUG" }
    symbols("On")
end

function projects.submit(project)
    local configSetup = { 
        [rythe.Configuration.RELEASE] = setupRelease,
        [rythe.Configuration.DEVELOPMENT] = setupDevelopment,
        [rythe.Configuration.DEBUG] = setupDebug        
    }

    for i, projectType in ipairs(project.types) do
        group(project.group)
        project(project.name)
        architecture(buildSettings.platform)
        toolset("clang")
        language("C++")
        cppdialect("C++23")
        defines(project.defines)
        for i, config in ipairs(rythe.Configuration) do
            configSetup[config]()
            kind(kindName(projectType, config))
        end
    end
end

function projects.scan(path)
    local srcDirs = {}
    local thirdpartyDirs = {}
    
    for i, dir in ipairs(os.matchdirs(path .. "**/src")) do
        if string.find(dir, "third_party") then
            thirdpartyDirs[#thirdpartyDirs + 1] = dir
        else
            srcDirs[#srcDirs + 1] = dir
        end
    end

    for i, dir in ipairs(srcDirs) do
        local projectPath = fs.parentPath(dir)
        local project = projects.load(projectPath)

        if project ~= nil then
            local message = "Found project:"
            message = message .. "\n  Group: " .. project.group
            message = message .. "\n  Name: " .. project.name

            message = message .. "\n  Assembly types:"
            for i, assem in ipairs(project.types) do
                message = message .. "\n\t" .. assem
            end

            if project.dependencies ~= nil then
                message = message .. "\n  Dependencies:"                
                for i, dep in ipairs(project.dependencies) do
                    message = message .. "\n\t" .. dep
                end
            end

            print(message)
        end
    end
end

return projects