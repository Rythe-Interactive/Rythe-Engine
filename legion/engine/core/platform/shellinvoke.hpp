#pragma once
#include <core/platform/platform.hpp>
#include <core/types/primitives.hpp>
#include <core/common/string_extra.hpp>
#include <core/logging/logging.hpp>

namespace legion::core
{
#if defined(LEGION_WINDOWS)
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
    inline bool ShellInvoke(const std::string& command, std::string& out, std::string& err)
    {
        const int READ_END = 0;
        const int WRITE_END = 1;

        int outfd[2] = { 0, 0 };
        int errfd[2] = { 0, 0 };

        auto cleanup = [&]() {
            close(outfd[READ_END]);
            close(outfd[WRITE_END]);

            close(errfd[READ_END]);
            close(errfd[WRITE_END]);
        };

        auto rc = pipe(outfd);
        if (rc < 0)
        {
            return false;
        }

        rc = pipe(errfd);
        if (rc < 0)
        {
            close(outfd[READ_END]);
            close(outfd[WRITE_END]);
            return false;
        }

        auto pid = fork();
        if (pid > 0) // PARENT
        {
            close(outfd[WRITE_END]);  // Parent does not write to stdout
            close(errfd[WRITE_END]);  // Parent does not write to stderr
        }
        else if (pid == 0) // CHILD
        {
            outfd[WRITE_END] = dup2(outfd[WRITE_END], STDOUT_FILENO);
            errfd[WRITE_END] = dup2(errfd[WRITE_END], STDERR_FILENO);
            if (outfd[WRITE_END] == -1 || errfd[WRITE_END] == -1)
                exit(EXIT_FAILURE);

            close(outfd[READ_END]);   // Child does not read from stdout
            close(errfd[READ_END]);   // Child does not read from stderr

            if (execl("/bin/bash", "bash", "-c", command.c_str(), nullptr) == -1)
                exit(EXIT_FAILURE);
            exit(EXIT_SUCCESS);
        }

        // PARENT
        if (pid < 0)
        {
            cleanup();
            return false;
        }

        int status = 0;
        waitpid(pid, &status, 0);

        std::array<char, 256> buffer;

        ssize_t bytes = 0;
        do
        {
            bytes = read(outfd[READ_END], buffer.data(), buffer.size());
            out.append(buffer.data(), bytes);
        } while (bytes > 0);

        do
        {
            bytes = read(errfd[READ_END], buffer.data(), buffer.size());
            err.append(buffer.data(), bytes);
        } while (bytes > 0);

        cleanup();
        return WEXITSTATUS(status) == EXIT_SUCCESS;
    }
#else
#error "fuck you"
#endif
}
