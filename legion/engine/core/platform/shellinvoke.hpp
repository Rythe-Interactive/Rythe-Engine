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

        auto seperator = command.find_first_of(' ');
        if (seperator == std::string::npos)
        {
            file = command;
        }
        else
        {
            file = command.substr(0, seperator);
        }

        SHELLEXECUTEINFOA shExecInfo{};
        shExecInfo.cbSize = sizeof(SHELLEXECUTEINFOA);
        shExecInfo.lpFile = file.c_str();
        shExecInfo.lpParameters = command.c_str();
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

        auto seperator = command.find_first_of(' ');
        if (seperator == std::string::npos)
        {
            file = command;
        }
        else
        {
            file = command.substr(0, seperator);
        }

        auto strvec = common::split_string_at<' '>(command);
        std::vector<const char*> paramsvec;

        for (auto s : strvec)
        {
            paramsvec.push_back(s.c_str());
        }

        paramsvec.push_back(nullptr);

        return execvp(file.c_str(), paramsvec.data()) != -1;
    }
#else
#error "fuck you"
#endif
}
