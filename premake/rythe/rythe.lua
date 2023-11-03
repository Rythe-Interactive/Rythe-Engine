include("_preload.lua")

premake.rythe = {
    Configuration = {
        RELEASE = 1,
        DEVELOPMENT = 2,
        DEBUG = 3
    },
    loadedProjects = {},
    buildSettings = {
        platform = "x86_64"
    }
}

local rythe = premake.rythe

local projects = dofile("projects.lua")

function rythe.test()
    projects.scan("./")
end

return rythe
