--[[
    Rythe uses a build system built on top of Premake5.
    Using the rythe premake module you can define projects and workspaces.
    Projects can either be manually defined, or automatically detected through .rythe_project files.
    .rythe_project files also allows you to customize the project, and report third-party dependencies that don't use the rythe build system.
]]

if(_ACTION ~= nil) then
    os.chdir(_MAIN_SCRIPT_DIR)

    filter("configurations:Debug-no-inline")
        defines { "RYTHE_DISABLE_ALWAYS_INLINE" }

    local r = require("premake/rythe")

    local workspace = {
        name = "rythe",
        location = "build/" .. _ACTION,
        configurations = { "Debug", "Debug-no-inline", "Development", "Release", "Debug-asan", "Release-profiling" }
    }

    r.configure({workspace})
end