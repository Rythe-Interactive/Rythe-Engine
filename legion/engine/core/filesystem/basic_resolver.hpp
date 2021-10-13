#pragma once
#include <core/filesystem/filesystem_resolver.hpp>
#include <filesystem>


#include "filemanip.hpp"
#include "core/common/string_extra.hpp"

#if !defined (LEGION_WINDOWS)
#include <unistd.h>
#endif

namespace legion::core::filesystem
{
    class basic_resolver final : public filesystem_resolver
    {
    public:

        explicit basic_resolver(std::string_view view) : m_root_path(view)
        {
            //make sure that the input string is of form
            // C:\something\folder
            // or
            // /somewhere/directory
            m_root_path = absolute(std::filesystem::path(m_root_path)).string();
        }

        basic_resolver(const basic_resolver& other) = default;
        basic_resolver(basic_resolver&& other) noexcept = default;
        basic_resolver& operator=(const basic_resolver& other) = default;
        basic_resolver& operator=(basic_resolver&& other) noexcept = default;

        ~basic_resolver() = default;

        L_NODISCARD filesystem_resolver* make() override
        {
            return new basic_resolver(m_root_path);
        }

        L_NODISCARD bool is_file() const noexcept override
        {
            return !is_directory() && is_valid_path();
        }

        L_NODISCARD bool is_directory() const noexcept override
        {
            const auto back = get_target().back();

            return (get_target().back() == '\\' || get_target().back() == '/')
                    && is_valid_path()
                    && !std::filesystem::is_regular_file(strpath_manip::subdir(m_root_path,get_target()));
        }

        L_NODISCARD bool is_valid_path() const noexcept override
        {
            auto full = strpath_manip::subdir(m_root_path,get_target());

            #if defined ( LEGION_WINDOWS )
                if(full.find_first_of("<>\"|?*") != std::string::npos) return false;
                if(full.back() == ' ' || full.back() == '.') return false;
               
            #elif defined( LEGION_LINUX )

            //linux don't give a shit

            #endif

            //All the control characters!
            if(full.find_first_of("\x00\x01\x02\x03\x04\x05\x06\x07\x09\x10\x11\x12\x13\x14\x15\x16\x17\x18\x19\x20\x21\x22\x23\x24\x25\x26\x27\x28\x29\x30\x31") 
                != std::string::npos) return false; // no characters that don't exists on the keyboard pls


            return true;
        }

        L_NODISCARD bool writeable() const noexcept override
        {
            if(!is_valid_path()) return false;

            const auto full = strpath_manip::subdir(m_root_path,get_target());

            if(is_file()){
                

                #if defined( LEGION_WINDOWS )

                const DWORD attr = GetFileAttributesA(full.c_str());
                if(attr != INVALID_FILE_ATTRIBUTES)
                {
                    if(attr & FILE_ATTRIBUTE_READONLY)
                    {
                        return false;
                    }
                    const HANDLE h = CreateFileA(full.c_str(),GENERIC_WRITE,NULL,NULL,OPEN_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
                    if(h == INVALID_HANDLE_VALUE)
                    {
                        if(GetLastError() == ERROR_SHARING_VIOLATION) return false;
                    }
                    CloseHandle(h);
                }
                return true;

                #else

                return access(full.c_str(),W_OK) != -1 || !exists();

                #endif
            }

            if(std::filesystem::is_directory(full) && std::filesystem::exists(full))
            {
                return false;
            }
            return true;
        }

        L_NODISCARD bool readable() const noexcept override
        {
            if(!exists()) return false;

            const auto full = strpath_manip::subdir(m_root_path,get_target());

            #if defined( LEGION_WINDOWS )
            const DWORD attr = GetFileAttributesA(full.c_str());
          
            const HANDLE h = CreateFileA(full.c_str(),GENERIC_READ,NULL,NULL,OPEN_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
            if(h == INVALID_HANDLE_VALUE)
            {
                if(GetLastError() == ERROR_SHARING_VIOLATION) return false;
            }
            CloseHandle(h);
            
            return true;

            #else
            return access(full.c_str(), R_OK) != -1;
            #endif
            
        }

        L_NODISCARD bool creatable() const noexcept override
        {
            return !exists() && writeable();
        }

        L_NODISCARD bool exists() const noexcept override
        {
            return std::filesystem::exists(strpath_manip::subdir(m_root_path,get_target()));
        }

        L_NODISCARD std::string get_absolute_path() const
        {
            return strpath_manip::subdir(m_root_path, get_target());
        }

        L_NODISCARD std::set<std::string> ls() const noexcept override
        {
            std::set<std::string> entries;
            for (const auto & entry : std::filesystem::directory_iterator(strpath_manip::subdir(m_root_path,get_target())))
            {
                entries.insert(get_identifier()+entry.path().string());
                //.relative_path()
            }
            return entries;
        }

        common::result<basic_resource, fs_error> get(interfaces::implement_signal_t) noexcept override
        {
            if(!exists()) return legion_fs_error("file does not exist, cannot read");
            if(!is_file()) return legion_fs_error("not a file");
            if(!readable()) return legion_fs_error("file not readable");
            return basic_resource(read_file(strpath_manip::subdir(m_root_path,get_target())));
        }

        common::result<const basic_resource, fs_error> get(interfaces::implement_signal_t) const noexcept override
        {
            if (!exists()) return legion_fs_error("file does not exist cannot read");
            if (!is_file()) return legion_fs_error("not a file");
            if (!readable()) return legion_fs_error("file not readable");
            return basic_resource(read_file(strpath_manip::subdir(m_root_path, get_target())));
        }

        common::result<void,fs_error> set(interfaces::implement_signal_t, const basic_resource& res) override
        {
            const auto full = strpath_manip::subdir(m_root_path,get_target());

            if(!writeable()) return legion_fs_error("file not writeable");
            if(is_directory())
            {
                if(!res.empty()) return legion_fs_error("attempted to create directory with data!");
                std::error_code code;
                std::filesystem::create_directories(full,code);
                if(code.value() != 0)
                {
                    return legion_fs_error(("std::filesystem bailed! " + code.message()).c_str());
                }
                return common::success;
            }

            std::error_code code;
            std::filesystem::create_directories(strpath_manip::parent(full),code);
            if(code.value() != 0)
            {
                return legion_fs_error(("std::filesystem bailed! " + code.message()).c_str());
            }

            write_file(full,res.get());

            return common::success;
        }

        void erase(interfaces::implement_signal_t) const noexcept override
        {
            //we really don't care if an error occured, but since this is noexcept we need to make sure that
            //std::filesystem::remove is too
            std::error_code code;
            std::filesystem::remove(strpath_manip::subdir(m_root_path,get_target()),code);
        }

    private:
        std::string m_root_path;

    };
};
