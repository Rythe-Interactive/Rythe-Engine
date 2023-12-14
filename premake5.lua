-- root workspace, all sub-project should be included
workspace "rythe"
    location(_ACTION)
    configurations { "Debug", "Development", "Release" }

os.chdir(_WORKING_DIR)

local r = require("premake/rythe")

r.configure()