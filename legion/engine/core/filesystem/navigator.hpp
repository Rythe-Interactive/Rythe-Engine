#pragma once
#include <string>

#include <core/common/result.hpp>
#include <core/filesystem/filesystem_resolver.hpp>

#include <utility>
#include <vector>



namespace legion::core::filesystem
{
    /** @class navigator
     *  @brief Resolves path strings to solutions of resolver + internal path.
     *  @note For internal use only.
     */
    class navigator
    {
    public:
        explicit navigator(std::string path) : m_path(std::move(path)) {}

        using solution = std::vector<std::pair<filesystem_resolver*,std::string>>;

        /**@brief Build a solution from the path provided in the constructor.
         * 
         * @param opt_root_domain New root domain when not present in path.
         * @return Ok of vector of pair of resolver pointers and paths or Err of fs_error when failure.
         */
        L_NODISCARD common::result<solution,fs_error> find_solution(const std::string& opt_root_domain ="") const;

    private:
        std::string m_path;
    };
}
