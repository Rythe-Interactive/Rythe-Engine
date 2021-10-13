#pragma once

#include <string>
#include <core/platform/platform.hpp>

#define SEP_WINDOWS 1
#define SEP_NIX 2

#ifdef _WIN32
#define OSSEP SEP_WINDOWS
#else
#define OSSEP SEP_NIX
#endif

namespace legion::core::filesystem
{
    class strpath_manip
    {
    public:
        
        /** @brief gets parent directory
         *  @param [in] p the path you want the parent of
         *  @return std::string path to parent directory
         */
        static std::string parent(const std::string& p);

        /** @brief gets a subdirectory
         *  @param [in] p the path you want to join with a subdirectory
         *  @param [in] sub the subdirectory of the path
         *  @return std::string path + subdirectory
         */
        static std::string subdir(const std::string& p, const std::string& sub);

        /** @brief sanitizes a path
         *  @param [in] p the path to sanitize
         *  @param [in] fail_on_fs_leave when true the operation will return an
         *              empty string when the filesystem terminates with ..
         *  @return std::string the sanitized path
         *  @note what does this do ?
         *        this will reduce surplus .. and . from the path, for example:
         *        ../cool_directory/no_so_cool_directory/./../hello_world
         *        becomes
         *        ../cool_directory/hello_world
         */
        static std::string sanitize(const std::string& p, bool fail_on_fs_leave = false);

        /** @brief makes a path "native" to the operating system
         *  @param [in] p the path you want to translate
         *  @return std::string the tranlsated path
         *  @note what does this do ?
         *        this will turn / into \ and \ into / depending on your os, for example:
         *          C:/hello_world\what/is\up
         *          becomes
         *          C:\hello_world\what\is\up (if your host is windows)
         */
        static std::string localize(const std::string& p);


        /** @brief does the same as @ref localize but without creating a new string
         *  @param [in/out] p the string you want to translate
         *  @return std::string& ref to p
         */
        static std::string& inline_localize(std::string& p);


        /** @brief gets the path separator that is "native" to the operating system */
        constexpr static char separator()
        {
            if constexpr (OSSEP == SEP_WINDOWS)
                return '\\';
            else return '/';
        }
    private:
        constexpr static char anti_separator()
        {
            if constexpr (OSSEP == SEP_NIX)
                return '\\';
            else return '/';
        }
    };
}

#undef OSSEP
#undef SEP_NIX
#undef SEP_WINDOWS
