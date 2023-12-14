local fs = {}

function fs.exists(file)
    local f = io.open(file, "rb")
    if f then io.close(f) end
    return f ~= nil
end

function fs.readLines(file)
    if not fs.exists(file) then return {} end

    local lines = {}
    for line in io.lines(file) do
        lines[#lines + 1] = line
    end

    return lines
end

function fs.parentPath(path)
    return string.match(path, "^(.+)[/\\]")
end

function fs.fileName(path)
    return string.match(path, "([^/\\]+)$")
end

function fs.rootName(path)
    return string.match(path, "^([^/\\]+)")
end

return fs