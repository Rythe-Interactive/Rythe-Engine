#pragma once
#include <string>

#include <core/common/result.hpp>
#include <core/filesystem/filesystem_resolver.hpp>

#include <utility>
#include <vector>



namespace args::core::filesystem
{
    class navigator
    {
    public:
        navigator(std::string path) : m_path(std::move(path)) {}

        using solution = std::vector<std::pair<filesystem_resolver*,std::string>>;

        A_NODISCARD common::result<solution,fs_error> find_solution(const std::string& opt_root_domain ="") const;

    private:
        std::string m_path;
    };
}
