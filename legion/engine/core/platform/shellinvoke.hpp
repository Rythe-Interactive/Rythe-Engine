#pragma once
#include <core/platform/platform.hpp>
#include <core/types/primitives.hpp>
#include <core/common/string_extra.hpp>
#include <core/logging/logging.hpp>

namespace legion::core
{
#if defined(LEGION_WINDOWS)
    namespace detail
    {

    }

    inline bool ShellInvoke(const std::string& command, std::string& out, std::string& err)
    {
        if (command.empty())
            return false;

        HANDLE childStdoutRd;
        HANDLE childStdoutWr;
        HANDLE childStderrRd;
        HANDLE childStderrWr;

        SECURITY_ATTRIBUTES saAttr; 
        saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
        saAttr.bInheritHandle = TRUE;
        saAttr.lpSecurityDescriptor = NULL;

        if (!CreatePipe(&childStdoutRd, &childStdoutWr, &saAttr, 0))
            return false;

        if (!SetHandleInformation(childStdoutRd, HANDLE_FLAG_INHERIT, 0))
            return false;

        if (!CreatePipe(&childStderrRd, &childStderrWr, &saAttr, 0))
            return false;

        if (!SetHandleInformation(childStderrRd, HANDLE_FLAG_INHERIT, 0))
            return false;

        char* cmd = new char[command.size() + 1];
        memcpy(cmd, command.c_str(), command.size());

        cmd[command.size()] = '\0';

        STARTUPINFOA si;
        PROCESS_INFORMATION pi;

        ZeroMemory(&si, sizeof(si));
        si.cb = sizeof(si);
        si.hStdError = childStderrWr;
        si.hStdOutput = childStdoutWr;
        si.dwFlags |= STARTF_USESTDHANDLES;
        ZeroMemory(&pi, sizeof(pi));

        log::trace("Executing command: {}", command);
        auto ret = CreateProcessA(NULL, cmd, NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi);

        CloseHandle(childStdoutWr);
        CloseHandle(childStderrWr);

        if (ret == FALSE)
        {
            CloseHandle(childStdoutRd);
            CloseHandle(childStderrRd);
            delete[] cmd;
            log::trace("Failed to execute command: {}", command);
            return false;
        }

        DWORD read;

        const size_type bufferSize = 512;

        char buffer[bufferSize];

        BOOL success = FALSE;
        while(true)
        {
            success = ReadFile(childStdoutRd, buffer, bufferSize, &read, NULL);
            if (!success || read == 0) break;

            out.append(buffer, read);
        }

        success = FALSE;
        while (true)
        {
            success = ReadFile(childStderrRd, buffer, bufferSize, &read, NULL);
            if (!success || read == 0) break;

            err.append(buffer, read);
        }

        CloseHandle(childStdoutRd);
        CloseHandle(childStderrRd);
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);

        delete[] cmd;
        log::trace("Successfully executed command: {}", command);
        return true;
    }

    inline bool ShellInvoke(const std::string& command)
    {
        std::string temp;
        return ShellInvoke(command, temp, temp);
    }

    inline bool ShellInvoke(const std::string& command, std::string& out)
    {
        std::string temp;
        return ShellInvoke(command, out, temp);
    }

#elif defined(LEGION_LINUX)
    inline bool ShellInvoke(const std::string& command)
    {
        if (command.empty())
            return false;

        std::string file;

        auto seperator = command.find_first_of(' ');
        if (seperator == std::string::npos)
        {
            file = command;
        }
        else
        {
            auto quote = command.find_first_of('\"');
            if (quote != std::string::npos && seperator > quote)
            {
                quote = command.find_first_of('\"', quote + 1);

                if (quote == std::string::npos || seperator > quote)
                    return false;

                seperator = quote + 1;
            }

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
