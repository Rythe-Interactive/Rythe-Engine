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

        std::vector<char*> paramsvec;

        auto iss = std::stringstream(command);
        auto str = std::string();

        while (iss >> str)
        {
            const char* r = str.c_str();
            char* x = new char[str.size()];
            memcpy(x, r, str.size());
            paramsvec.push_back(x);
        }

        paramsvec.push_back(nullptr);

        bool ret = execvp(file.c_str(), paramsvec.data()) != -1;

        for (auto ptr : paramsvec)
        {
            if (ptr)
                delete[] ptr;
        }

        return ret;
    }
#else
#error "fuck you"
#endif
}
