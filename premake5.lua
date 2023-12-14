-- root workspace, all sub-project should be included
workspace "rythe"
    location(_ACTION)
    configurations { "Debug", "Release" }

os.chdir(_WORKING_DIR)

local r = require("premake/rythe")

r.configure()

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