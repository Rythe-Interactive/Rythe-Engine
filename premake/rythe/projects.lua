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

local function projectTypeFilesDir(projectType, namespace)
    if projectType == "test" then
        return "/tests/"
    elseif projectType == "editor" then
        return "/editor/"
    end

    if namespace == "" or namespace == nil then
        return "/src/"
    end

    return "/src/" .. namespace .. "/"
end

local function isProjectTypeMainType(projectType)
    if projectType == "test" then
        return false
    elseif projectType == "editor" then
        return false
    end

    return true
end

local function printTable(name, table, indent)
    print(indent .. name .. ":")
    indent = indent .. "\t"
    for key, value in pairs(table) do
        if type(value) == "table" then
            printTable(key, value, indent)
        elseif type(value) == "function" then
            print(indent .. key .. ": " .. "function")
        else
            print(indent .. key .. ": " .. value)
        end
    end
    indent = indent:sub(-1)
end

local function loadProject(projectId, project, projectPath, name, projectType)
    if project.alias == nil then
        project.alias = name
    end

    if project.namespace == nil then
        project.namespace = name
    end

    if project.types == nil then
        project.types = { projectType }
    else
        project.types[1] = projectType
    end

    if project.fast_up_to_date_check == nil then
        project.fast_up_to_date_check = true
    end

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
            
    if loadedProjects[projectId] ~= nil then
        return loadedProjects[projectId]
    end

    if projectFile == nil then
        print("Could not find project \"" .. group .. "/" .. name .. "\"")
        return nil
    end

    if thirdPartyFile ~= nil then
        local thirdParties = dofile(thirdPartyFile)

        for i, thirdParty in ipairs(thirdParties) do
            thirdParty.src = thirdPartyFile

            if thirdParty.init ~= nil then
                thirdParty = thirdParty:init(ctx)
            end
            
            if thirdParty == nil then
                print("Could not initialize a third party dependency of project \"" .. group .. "/" .. name .. "\"")
                return nil
            end

            local thirdPartyType = "library"

            if not utils.tableIsEmpty(thirdParty.types) then
                thirdPartyType = thirdParty.types[1]
            end

            local thirdPartyId = getProjectId(thirdParty.group, thirdParty.name)

            if not isThirdPartyProject(thirdPartyId) then
                thirdParty.group = "third_party/" .. thirdParty.group
                thirdPartyId = getProjectId(thirdParty.group, thirdParty.name)
            end

            if thirdParty.location == nil then
                thirdParty.location = projectPath .. "/third_party/" .. thirdParty.name
            end

            thirdParty = loadProject(thirdPartyId, thirdParty, thirdParty.location, thirdParty.name, thirdPartyType)
        end
    end

    local project = dofile(projectFile)

    project.group = group
    project.name = name
    project.src = projectFile
    if project.init ~= nil then
        project = project:init(ctx)
    end

    if project == nil then
        print("Could not initialize project \"" .. group .. "/" .. name .. "\"")
        return nil
    end

    if not utils.tableIsEmpty(project.types) then
        projectType = project.types[1]
    end

    return loadProject(projectId, project, projectPath, name, projectType)
end

local function appendConfigSuffix(linkTargets, config)
    local suffix = rythe.targetSuffix(config)
    local copy = {}
    for i, target in ipairs(linkTargets) do
        copy[i] = target .. suffix
    end

    return copy
end

local function setupRelease(projectType, linkTargets)
    filter("configurations:Release")
        defines { "NDEBUG" }
        optimize("Full")
        kind(kindName(projectType, rythe.Configuration.RELEASE))
        links(appendConfigSuffix(linkTargets, rythe.Configuration.RELEASE))
end

local function setupDevelopment(projectType, linkTargets)
    filter("configurations:Development")
        defines { "DEBUG" }
        optimize("Debug")
        inlining("Explicit")
        symbols("On")
        kind(kindName(projectType, rythe.Configuration.DEVELOPMENT))
        links(appendConfigSuffix(linkTargets, rythe.Configuration.DEVELOPMENT))
end

local function setupDebug(projectType, linkTargets)
    filter("configurations:Debug")
        defines { "DEBUG" }
        symbols("On")
        kind(kindName(projectType, rythe.Configuration.DEBUG))
        links(appendConfigSuffix(linkTargets, rythe.Configuration.DEBUG))
end

local function getDepsRecursive(project, projectType)
    local deps = project.dependencies
    
    if deps == nil then
        deps = {}
    end

    if projectType == "test" then
        deps[#deps + 1] = "third_party/catch2"
    end

    local copy = utils.copyTable(deps)

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

            if not fs.exists(path .. "/") then
                path = _WORKING_DIR .. "/libraries/third_party/" .. thirdPartyProject.name
            end

            if fs.exists(path .. "/") then
                thirdPartyProject.files = {}
                    
                thirdPartyProject.additional_include_dirs = {
                    path .. "/src",
                    path .. "/include"
                }

                thirdPartyProject.additional_external_include_dirs = {
                    path .. "/src",
                    path .. "/include"
                }

                local srcDir = path .. "/src/" .. thirdPartyProject.name .. "/"
                if fs.exists(srcDir) then
                    thirdPartyProject.files[#thirdPartyProject.files + 1] = srcDir .. "**"
                end
                
                local includeDir = path .. "/include/" .. thirdPartyProject.name .. "/"
                if fs.exists(includeDir) then
                    thirdPartyProject.files[#thirdPartyProject.files + 1] = includeDir .. "**"
                end

                if utils.tableIsEmpty(thirdPartyProject.files) then
                    thirdPartyProject.files = {
                        path .. "/src/**",
                        path .. "/include/**"
                    }
                end

                thirdPartyProject.src = project.src

                if depType == nil then
                    depType = "library"
                end

                depProject = loadProject(depId, thirdPartyProject, path, thirdPartyProject.name, depType)
            end
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

function projects.resolveDeps(proj)
    for i, projectType in ipairs(proj.types) do
        getDepsRecursive(proj, projectType)
    end
end

function projects.submit(proj)
    local configSetup = { 
        [rythe.Configuration.RELEASE] = setupRelease,
        [rythe.Configuration.DEVELOPMENT] = setupDevelopment,
        [rythe.Configuration.DEBUG] = setupDebug        
    }

    for i, projectType in ipairs(proj.types) do
        local fullGroupPath = projectTypeGroupPrefix(projectType) .. proj.group
        local binDir = "build/" .. _ACTION .. "/bin/"
        print("Building " .. proj.name .. ": " .. projectType)

        group(fullGroupPath)
        project(proj.alias .. projectNameSuffix(projectType))
            filename(proj.alias .. projectNameSuffix(projectType))
            location("build/" .. _ACTION .. "/" .. proj.group)

            fastuptodate(proj.fast_up_to_date_check)

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
            
            local libDirs = {}
            local linkTargets = {}
            local externalIncludeDirs = {}
            
            if not utils.tableIsEmpty(allDeps) then
                local depNames = {}
                for i, dep in ipairs(allDeps) do
                    local assemblyId, scope = getDepAssemblyAndScope(dep)
                    local depProject, depId, depType = findAssembly(assemblyId)

                    if depProject ~= nil then
                        externalIncludeDirs[#externalIncludeDirs + 1] = depProject.location .. projectTypeFilesDir(depType, "")
                        
                        if isThirdPartyProject(depId) then
                            externalIncludeDirs[#externalIncludeDirs + 1] = depProject.location .. "/include/"
                            externalIncludeDirs[#externalIncludeDirs + 1] = depProject.location .. "/"
                        end

                        depNames[#depNames + 1] = depProject.alias .. projectNameSuffix(depType)
                        
                        allDefines[#allDefines + 1] = depProject.group == "" and string.upper(depProject.alias) .. "=1" or string.upper(string.gsub(depProject.group, "[/\\]", "_")) .. "_" .. string.upper(depProject.alias) .. "=1"

                        libDirs[#libDirs + 1] = binDir .. depProject.group .. "/" .. depProject.name
                        
                        if depType ~= "header-only" then
                            linkTargets[#linkTargets + 1] = depProject.alias .. projectNameSuffix(depType)
                        end
                    else
                        print("\tDependency \"" .. depId .. "\" was not found")
                    end
                end
                
                dependson(depNames)
            end
            
            architecture(buildSettings.platform)
            
            local targetDir = binDir .. proj.group .. "/" .. proj.name
            targetdir(targetDir)
            objdir(binDir .. "obj")
            
            if projectType ~= "util" then
                if not utils.tableIsEmpty(externalIncludeDirs) then
                    externalincludedirs(externalIncludeDirs)
                end

                if not utils.tableIsEmpty(libDirs) then
                    libdirs(libDirs)
                end

                if not utils.tableIsEmpty(proj.additional_include_dirs) then
                    includedirs(proj.additional_include_dirs)
                end

                if not utils.tableIsEmpty(proj.additional_external_include_dirs) then
                    externalincludedirs(proj.additional_external_include_dirs)
                end

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
                    filePatterns[#filePatterns + 1] = proj.location .. projectTypeFilesDir(projectType, proj.namespace) .. string.sub(pattern, 3)
                end
            end

            if projectType == "test" then
                filePatterns[#filePatterns + 1] = _WORKING_DIR .. "/utils/test utils/**"

                vpaths({ ["test utils"] = _WORKING_DIR .. "/utils/test utils/**" })
            end

            filePatterns[#filePatterns + 1] = proj.src

            vpaths({ ["*"] = { proj.location .. projectTypeFilesDir(projectType, proj.namespace), fs.parentPath(proj.src) }})
            files(filePatterns)

            if not utils.tableIsEmpty(proj.exclude_files) then
                local excludePatterns = {}
                for i, pattern in ipairs(proj.exclude_files) do
                    if string.find(pattern, "^(%.[/\\])") == nil then
                        excludePatterns[#excludePatterns + 1] = pattern
                    else
                        excludePatterns[#excludePatterns + 1] = proj.location .. projectTypeFilesDir(projectType, proj.namespace) .. string.sub(pattern, 3)
                    end
                end

                removefiles(excludePatterns)
            end

            if projectType == "util" then
                kind("Utility")
            else
                for i, config in pairs(rythe.Configuration) do
                    configSetup[config](projectType, linkTargets)
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

    local sourceProjects = utils.copyTable(loadedProjects)
    for projectId, project in pairs(sourceProjects) do
        projects.resolveDeps(project)
    end

    for projectId, project in pairs(loadedProjects) do
        projects.submit(project)
    end
end

return projects