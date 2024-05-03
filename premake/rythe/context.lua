local context = {}

local utils = dofile("utils.lua")

local function hasFilter(list, item)
	if list then
		if string.contains(list, "-" .. item) then
            return false
        end

		return string.contains(list, item)
	end

	return true
end

-- Gets the command that was used to call premake with
function context.getCommand()
	local cmd = "tools\\premake5 " .. _ACTION
	for k,v in pairs(_OPTIONS) do
		cmd = cmd .. "--" .. k .. "=" .. tostring(v) .. " "
	end

	return utils.trim(cmd)
end

function context.hasConfiguration(configuration)
	return hasFilter(_OPTIONS["configurations"], configuration)
end

-- Module names: e.g. core, graphics, audio
function context.hasModule(module)
	return hasFilter(_OPTIONS["modules"], module)
end

-- Project types are e.g. test, module, application, editor, libary, header-only, util
function context.hasProjectType(projectType)
    return hasFilter(_OPTIONS["types"], projectType)
end

-- Project groups are e.g. rythe, deps, my_plugin, my_game, etc
function context.hasProjectGroup(projectGroup)
    return hasFilter(_OPTIONS["groups"], projectGroup)
end

-- Links as shared library
function context.linkShared()
    return _OPTIONS["shared"] and not _OPTIONS["static"]
end

-- If no link target is defined, then static is chosen
function context.linkStatic()
    return _OPTIONS["static"] or not context.linkShared()
end

function context.linkTarget()
	return (context.linkStatic() and "StaticLib") or "SharedLib"
end

-- Tag to place at the end of a solution name
function context.solutionTag()
    local tag = _OPTIONS["tag"]
	if tag then return "-" .. tag end
	return ""
end

return context