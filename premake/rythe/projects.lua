local fs = dofile("filesystem.lua")
local ctx = dofile("context.lua")
local utils = dofile("utils.lua")

local rythe = premake.rythe
local loadedProjects = rythe.loadedProjects
local buildSettings = rythe.buildSettings

local projects = {}

local function folderToProjectType(projectFolder)
    if projectFolder == "applications" then
        return "application"
    elseif projectFolder == "modules" then
        return "module"
    elseif projectFolder == "libraries" then
        return "library"
    end

    return projectFolder
end

local function find(projectPath)
    local projectName = fs.fileName(projectPath)
    local projectType = fs.rootName(projectPath)
    local group = fs.parentPath(projectPath)

    local projectTypeLength = string.len(projectType) + 2
    if string.len(group) > projectTypeLength then
        group = string.sub(group, projectTypeLength)
    else
        group = ""
    end

    local projectFile = projectPath .. "/.rythe_project"

    return projectFile, group, projectName, folderToProjectType(projectType)
end

local function projectIdPostfix(projectType)
    if projectType == "test" then
        return ":test"
    elseif projectType == "editor" then
        return ":editor"
    end

    return ""
end

local function getProjectId(group, projectName)
    return group == "" and projectName or group .. "/" .. projectName
end

local function findAssembly(assemblyId)
    local projectId = string.match(assemblyId, "^([^:]+)")
    local projectType = string.sub(assemblyId, string.len(projectId) + 2)
    local project = loadedProjects[projectId]

    if projectType == "" then
        
        if project == nil then
            return nil, projectId, nil
        else
            return project, projectId, project.types[1]
        end
    end

    return project, projectId, projectType
end

local function kindName(projectType, config)
    if projectType == "module" then
        return ctx.linkTarget()
    elseif projectType == "test" then
        return "ConsoleApp"
    elseif projectType == "editor" then
        return "SharedLib"
    elseif projectType == "application" then
        if config == rythe.Configuration.RELEASE then
            return "WindowedApp"
        else
            return "ConsoleApp"
        end
    elseif projectType == "library" then
        return "StaticLib"
    elseif projectType == "header-only" then
        return "SharedItems"
    elseif projectType == "util" then
        return "Utility"
    end
    assert(false, "Unknown project type: \"" .. projectType .. "\"")
end

local function projectTypeGroupPrefix(projectType)
    if projectType == "util" then
        return "1 - utils/"
    elseif projectType == "application" then
        return "2 - applications/"
    elseif projectType == "module" then
        return "3 - modules/"
    elseif projectType == "editor" then
        return "4 - editor/"
    elseif projectType == "library" or projectType == "header-only" then
        return "5 - libraries/"
    elseif projectType == "test" then
        return "6 - tests/"
    end

    assert(false, "Unknown project type: \"" .. projectType .. "\"")
end

local function projectNamePostfix(projectType)
    if projectType == "module" then
        return "-module"
    elseif projectType == "test" then
        return "-test"
    elseif projectType == "application" then
        return "-application"
    elseif projectType == "editor" then
        return "-editor"
    end

    return ""
end

local function projectTypeFilesDir(projectType)
    if projectType == "test" then
        return "/tests/"
    elseif projectType == "editor" then
        return "/editor/"
    end

    return "/src/"
end

local function isProjectTypeMainType(projectType)
    if projectType == "test" then
        return false
    elseif projectType == "editor" then
        return false
    end

    return true
end

function projects.load(projectPath)
    
    local projectFile, group, name, projectType = find(projectPath)
    local projectId = getProjectId(group, name)
    
    local project = loadedProjects[projectId]
        
    if project ~= nil then
        return project
    end

    print("Loading project at \"" .. projectPath .. "\"")

    if projectFile == nil then
        print("Could not find project \"" .. group .. "/" .. name .. "\"")
        return nil
    end

    project = dofile(projectFile)

    if project == nil then
        print("Could not initialize project \"" .. group .. "/" .. name .. "\"")
        return nil
    end

    project.group = group
    project.name = name
    
    if project.alias == nil then
        project.alias = name
    end

    if project.namespace == nil then
        project.namespace = name
    end

    project.types = { projectType }

    if not utils.tableIsEmpty(project.additional_types) then
        project.types = utils.concatTables(project.types, project.additional_types)
    end

    if utils.tableIsEmpty(project.defines) then
        project.defines = { "PROJECT_NAME=" .. project.alias, "PROJECT_FULL_NAME=" .. project.name, "PROJECT_NAMESPACE=" .. project.namespace }
    else
        project.defines[#project.defines +1 ] = "PROJECT_NAME=" .. project.alias
        project.defines[#project.defines +1 ] = "PROJECT_FULL_NAME=" .. project.name
        project.defines[#project.defines +1 ] = "PROJECT_NAMESPACE=" .. project.namespace
    end

    project.src = projectFile
    project.location = projectPath

    if project.files == nil then -- files can be an empty table if no files need to be loaded
        project.files = { "**" }
    end

    loadedProjects[projectId] = project

    return project
end

local function setupRelease(projectType)
    filter("configurations:Release")
        defines { "NDEBUG" }
        optimize("Full")
        kind(kindName(projectType, rythe.Configuration.RELEASE))
end

local function setupDevelopment(projectType)
    filter("configurations:Development")
        defines { "DEBUG" }
        optimize("Debug")
        inlining("Explicit")
        symbols("On")
        kind(kindName(projectType, rythe.Configuration.DEVELOPMENT))
end

local function setupDebug(projectType)
    filter("configurations:Debug")
        defines { "DEBUG" }
        symbols("On")
        kind(kindName(projectType, rythe.Configuration.DEBUG))
end

function projects.submit(proj)
    local configSetup = { 
        [rythe.Configuration.RELEASE] = setupRelease,
        [rythe.Configuration.DEVELOPMENT] = setupDevelopment,
        [rythe.Configuration.DEBUG] = setupDebug        
    }

    for i, projectType in ipairs(proj.types) do
        local fullGroupPath = projectTypeGroupPrefix(projectType) .. proj.group
        local binDir = _ACTION .. "/bin/"
        print("Building " .. proj.name .. ": " .. projectType)

        group(fullGroupPath)
        project(proj.alias .. projectNamePostfix(projectType))        
            filename(proj.alias)
            location(_ACTION .. "/" .. fullGroupPath)
            targetdir(binDir .. fullGroupPath)
            objdir(binDir .. "obj")

            local allDeps = proj.dependencies

            if not isProjectTypeMainType(projectType) then
                if utils.tableIsEmpty(allDeps) then
                    allDeps = { getProjectId(proj.group, proj.name) }
                else
                    allDeps = utils.concatTables({ getProjectId(proj.group, proj.name) }, allDeps)
                end
            end

            if not utils.tableIsEmpty(allDeps) then
                local libDirs = {}
                local externalIncludeDirs = {}
                local depNames = {}

                for i, dep in ipairs(allDeps) do
                    depProject, depId, depType = findAssembly(dep)

                    if depProject ~= nil then
                        print("\tDependency: " .. depId .. " - " .. depType)
                        externalIncludeDirs[#externalIncludeDirs + 1] = depProject.location .. "/" .. projectTypeFilesDir(depType)
                        depNames[#depNames + 1] = depProject.alias .. projectNamePostfix(depType)
                    else
                        print("\tDependency \"" .. depId .. "\" was not found")
                    end
                end

                dependson(depNames)
                externalincludedirs(externalIncludeDirs)
                libdirs(libDirs)
            end

            architecture(buildSettings.platform)
            toolset(buildSettings.toolset)
            language("C++")
            cppdialect(buildSettings.cppVersion)

            local allDefines = proj.defines

            if allDefines == nil then
                allDefines = {}
            end

            for projectId, project in pairs(loadedProjects) do
                allDefines[#allDefines + 1] = project.group == "" and string.upper(project.alias) .. "=1" or string.upper(string.gsub(project.group, "[/\\]", "_")) .. "_" .. string.upper(project.alias) .. "=1"
            end

            defines(allDefines)

            local filePatterns = {}
            for i, pattern in ipairs(proj.files) do
                filePatterns[#filePatterns + 1] =  proj.location .. projectTypeFilesDir(projectType) .. proj.namespace .. "/" .. pattern
            end

            files(filePatterns)
            
            if not utils.tableIsEmpty(proj.exclude_files) then
                local excludePatterns = {}
                for i, pattern in ipairs(proj.exclude_files) do
                    excludePatterns[#excludePatterns + 1] =  proj.location .. projectTypeFilesDir(projectType) .. proj.namespace .. "/" .. pattern
                end

                removefiles(excludePatterns)
            end
            
            for i, config in pairs(rythe.Configuration) do
                configSetup[config](projectType)
            end

        filter("")
    end

    group("")
end

function projects.scan(path)
    local srcDirs = {}
    local thirdpartyDirs = {}
    
    for i, file in ipairs(os.matchfiles(path .. "**/.rythe_project")) do
        if string.find(file, "third_party") then
            thirdpartyDirs[#thirdpartyDirs + 1] = fs.parentPath(file)
        else
            srcDirs[#srcDirs + 1] = fs.parentPath(file)
        end
    end

    for i, dir in ipairs(srcDirs) do
        local projectPath = dir
        local project = projects.load(projectPath)

        if project ~= nil then
            local message = "Found project:"
            message = message .. "\n  Group: " .. project.group
            message = message .. "\n  Name: " .. project.name

            message = message .. "\n  Assembly types:"
            for i, assem in ipairs(project.types) do
                message = message .. "\n\t" .. assem
            end

            if not utils.tableIsEmpty(project.dependencies) then
                message = message .. "\n  Dependencies:"                
                for i, dep in ipairs(project.dependencies) do
                    message = message .. "\n\t" .. dep
                end
            end

            message = message .. "\n  Location: " .. project.location
            message  = message .. "\n  Src: " .. project.src

            if not utils.tableIsEmpty(project.defines) then
                message = message .. "\n  Defines:"
                for i, def in ipairs(project.defines) do
                    message = message .. "\n\t" .. def
                end
            end

            print(message)
        end
    end
    
    for projectId, project in pairs(loadedProjects) do
        projects.submit(project)
    end
end

return projects