local utils = {}

function utils.trim(s)
	return (s:gsub("^%s*(.-)%s*$", "%1"))
end

function utils.concatTables(lhs, rhs)
	for i=1, #rhs do
		lhs[#lhs + 1] = rhs[i]
	end
	return lhs
end

function utils.tableIsEmpty(t)
	return t == nil or next(t) == nil
end

function utils.stringEndsWith(str, suffix)
	return str:sub(-#suffix) == suffix
end

function utils.copyTable(t)
	local copy = {}
	for key, value in pairs(t) do
		copy[key] = value
	end
	return copy
end

return utils