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
    elseif projectFolder == "utils" then
        return "util"
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

    local thirdPartyFile = projectPath .. "/.rythe_third_party"
    if not fs.exists(thirdPartyFile) then
        thirdPartyFile = nil
    end

    return projectFile, thirdPartyFile, group, projectName, folderToProjectType(projectType)
end

local function projectIdSuffix(projectType)
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

local function getDepAssemblyAndScope(dependency)
    local scope = string.match(dependency, "([^%s]+[%s]+)")
    local assemblyId = dependency
    
    if scope ~= "" and scope ~= nil then
        assemblyId = string.sub(dependency, string.len(scope) + 1)
        
        scope = string.gsub(scope, "%s+", "")
        if scope ~= "public" and scope ~= "private" then
            return nil, nil
        end

    else
        scope = "private"
    end

    return assemblyId, scope
end

local function isThirdPartyProject(projectId)
    return string.find(projectId, "^(third_party)") ~= nil
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

local function projectNameSuffix(projectType)
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

local function loadProject(projectId, project, projectFile, projectPath, name, projectType)    
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
        project.files = { "./**" }
    end

    loadedProjects[projectId] = project

    return project
end

function projects.load(projectPath)
    
    local projectFile, thirdPartyFile, group, name, projectType = find(projectPath)
    local projectId = getProjectId(group, name)
    
    local project = loadedProjects[projectId]
        
    if project ~= nil then
        return project
    end

    if projectFile == nil then
        print("Could not find project \"" .. group .. "/" .. name .. "\"")
        return nil
    end

    if thirdPartyFile ~= nil then
        local thirdParties = dofile(thirdPartyFile)

        for i, thirdParty in ipairs(thirdParties) do
            if thirdParty.init ~= nil then
                thirdParty = thirdParty:init(ctx)
            end

            local thirdPartyId = getProjectId(thirdParty.group, thirdParty.name)

            if not isThirdPartyProject(thirdPartyId) then
                thirdParty.group = "third_party/" .. thirdParty.group
                thirdPartyId = getProjectId(thirdParty.group, thirdParty.name)
            end

            if thirdParty.location == nil then
                thirdParty.location = projectPath .. "/third_party/" .. thirdParty.name
            end

            loadProject(thirdPartyId, thirdParty, thirdPartyFile, thirdParty.location, thirdParty.name, "library")
        end
    end

    project = dofile(projectFile)

    if project.init ~= nil then
        project = project:init(ctx)
    end

    if project == nil then
        print("Could not initialize project \"" .. group .. "/" .. name .. "\"")
        return nil
    end

    project.group = group
    project.name = name

    return loadProject(projectId, project, projectFile, projectPath, name, projectType)
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

local function getDepsRecursive(project, projectType)
    local deps = project.dependencies
    
    if deps == nil then
        deps = {}
    end

    local copy = deps

    local set = {}

    for i, dep in ipairs(deps) do
        set[dep] = true
    end

    for i, dep in ipairs(deps) do
        local assemblyId, scope = getDepAssemblyAndScope(dep)
        local depProject, depId, depType = findAssembly(assemblyId)

        if depProject == nil and isThirdPartyProject(depId) then
            local thirdPartyProject = {
                group = fs.parentPath(depId),
                name = fs.fileName(depId)
            }

            local path = project.location .. "/third_party/" .. thirdPartyProject.name
            thirdPartyProject.files = {
                path .. "/src/**",
                path .. "/include/**"
            }

            depProject = loadProject(depId, thirdPartyProject, project.src, path, thirdPartyProject.name, "library")
            depType = "library"
        end

        if depProject ~= nil then
            local newDeps = getDepsRecursive(depProject, depType)
            for i, newDep in ipairs(newDeps) do
                local newDepAssemblyId, newDepScope = getDepAssemblyAndScope(newDep)

                if newDepScope == "public" and set[newDep] == nil then
                    set[newDep] = true
                    copy[#copy + 1] = newDep
                end
            end
        end
    end

    if not isProjectTypeMainType(projectType) then
        if utils.tableIsEmpty(copy) then
            copy = { getProjectId(project.group, project.name) }
        else
            copy = utils.concatTables({ getProjectId(project.group, project.name) }, copy)
        end
    end

    return copy
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
        project(proj.alias .. projectNameSuffix(projectType))
            filename(proj.alias .. projectNameSuffix(projectType))
            location(_ACTION .. "/" .. proj.group)

            if proj.pre_build ~= nil then                
                prebuildcommands(proj.pre_build)
            end

            if proj.post_build ~= nil then                
                postbuildcommands(proj.post_build)
            end

            if proj.pre_link ~= nil then                
                prelinkcommands(proj.pre_link)
            end

            local allDeps = getDepsRecursive(proj, projectType)
            local allDefines = proj.defines

            if allDefines == nil then
                allDefines = {}
            end

            if not utils.tableIsEmpty(allDeps) then
                local libDirs = {}
                local externalIncludeDirs = {}
                local depNames = {}

                for i, dep in ipairs(allDeps) do
                    local assemblyId, scope = getDepAssemblyAndScope(dep)
                    local depProject, depId, depType = findAssembly(assemblyId)

                    if depProject ~= nil then
                        externalIncludeDirs[#externalIncludeDirs + 1] = depProject.location .. "/" .. projectTypeFilesDir(depType)
                        
                        depNames[#depNames + 1] = depProject.alias .. projectNameSuffix(depType)
                        
                        allDefines[#allDefines + 1] = depProject.group == "" and string.upper(depProject.alias) .. "=1" or string.upper(string.gsub(depProject.group, "[/\\]", "_")) .. "_" .. string.upper(depProject.alias) .. "=1"
                    else
                        print("\tDependency \"" .. depId .. "\" was not found")
                    end
                end

                dependson(depNames)
                externalincludedirs(externalIncludeDirs)
                libdirs(libDirs)
            end
            
            architecture(buildSettings.platform)

            if projectType ~= "util" then
                targetdir(binDir .. proj.group .. "/" .. proj.name .. projectNameSuffix(projectType))
                objdir(binDir .. "obj")
                defines(allDefines)
                
                toolset(buildSettings.toolset)
                language("C++")
                cppdialect(buildSettings.cppVersion)
            end

            local filePatterns = {}
            for i, pattern in ipairs(proj.files) do
                if string.find(pattern, "^(%.[/\\])") == nil then
                    filePatterns[#filePatterns + 1] = pattern
                else
                    filePatterns[#filePatterns + 1] = proj.location .. projectTypeFilesDir(projectType) .. proj.namespace .. "/" .. string.sub(pattern, 3)
                end
            end

            files(filePatterns)
            
            if not utils.tableIsEmpty(proj.exclude_files) then
                local excludePatterns = {}
                for i, pattern in ipairs(proj.exclude_files) do
                    if string.find(pattern, "^(%.[/\\])") == nil then
                        excludePatterns[#excludePatterns + 1] = pattern
                    else
                        excludePatterns[#excludePatterns + 1] = proj.location .. projectTypeFilesDir(projectType) .. proj.namespace .. "/" .. string.sub(pattern, 3)
                    end
                end

                removefiles(excludePatterns)
            end

            if projectType == "util" then
                kind("Utility")
            else
                for i, config in pairs(rythe.Configuration) do
                    configSetup[config](projectType)
                end
            end
        filter("")
    end

    group("")
end

function projects.scan(path)
    local srcDirs = {}

    for i, file in ipairs(os.matchfiles(path .. "**/.rythe_project")) do        
        srcDirs[#srcDirs + 1] = fs.parentPath(file)
    end

    for i, dir in ipairs(srcDirs) do
        local project = projects.load(dir)
    end
    
    for projectId, project in pairs(loadedProjects) do
        projects.submit(project)
    end
end

return projects