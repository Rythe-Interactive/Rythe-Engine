include("_preload.lua")

premake.rythe = {
    Configuration = {
        RELEASE = 1,
        DEVELOPMENT = 2,
        DEBUG = 3
    },
    loadedProjects = {},
    buildSettings = {
        platform = "x86_64",
        toolset = "clang",
        cppVersion = "C++20"
    }
}

local rythe = premake.rythe

local projects = dofile("projects.lua")

function rythe.configName(config)
    local configNames = { 
        [rythe.Configuration.RELEASE] = "Release",
        [rythe.Configuration.DEVELOPMENT] = "Development",
        [rythe.Configuration.DEBUG] = "Debug"        
    }

    return configNames[config]
end

function rythe.targetSuffix(config)
    local configSuffix = { 
        [rythe.Configuration.RELEASE] = "",
        [rythe.Configuration.DEVELOPMENT] = "-dev",
        [rythe.Configuration.DEBUG] = "-debug"        
    }

    return configSuffix[config]
end

function rythe.configure()
    filter { "configurations:Debug" }
        targetsuffix(rythe.targetSuffix(rythe.Configuration.DEBUG))
    filter { "configurations:Development" }
        targetsuffix(rythe.targetSuffix(rythe.Configuration.DEVELOPMENT))

    projects.scan("./")
end

return rythe
