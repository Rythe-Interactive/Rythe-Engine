#pragma once
#include <core/platform/platform.hpp>
#include <core/types/primitives.hpp>
#include <core/common/string_extra.hpp>

namespace legion::core
{
#if defined(LEGION_WINDOWS)
    inline bool ShellInvoke(const std::string& command)
    {
        std::string file;
        std::string params;

        auto seperator = command.find_first_of(' ');
        if (seperator == std::string::npos)
        {
            file = command;
            params = "";
        }
        else
        {
            file = command.substr(0, seperator);
            params = command.substr(seperator);
        }

        SHELLEXECUTEINFOA shExecInfo{};
        shExecInfo.cbSize = sizeof(SHELLEXECUTEINFOA);
        shExecInfo.lpFile = file.c_str();
        shExecInfo.lpParameters = params.c_str();
        shExecInfo.nShow = SW_SHOWNORMAL;

        CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);

        auto ret = ShellExecuteExA(&shExecInfo);

        CoUninitialize();

        return ret == TRUE;
    }
#elif defined(LEGION_LINUX)
    inline bool ShellInvoke(const std::string& command)
    {
        std::string file;
        std::string params;
        std::vector<char> paramsvec;

        auto seperator = command.find_first_of(' ');
        if (seperator == std::string::npos)
        {
            file = command;
            params = "";
        }
        else
        {
            file = command.substr(0, seperator);
            params = command.substr(seperator);
        }

        paramsvec.assign(params.begin(), params.end());
        common::replace_items(paramsvec, " ", "\0");
        paramsvec.push_back('\0');

        return execvp(file.c_str(), paramsvec.data()) != -1;
    }
#else
#error "fuck you"
#endif
}
