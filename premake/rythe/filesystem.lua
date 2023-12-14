local fs = {}

function fs.exists(file)
    local ok, err, code = os.rename(file, file)
    if not ok then
        if code == 13 or code == 32 or code == 5 then
            -- Permission denied, but it exists
            return true, nil, nil
        end
        return false, err, code
   end
   return true, nil, nil
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