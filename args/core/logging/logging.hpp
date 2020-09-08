#pragma once

#define SPDLOG_HEADER_ONLY
#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/sinks/rotating_file_sink.h"
#include "spdlog/pattern_formatter.h"
#include <core/types/type_util.hpp>

namespace args::core::log
{

    const static inline std::chrono::time_point<std::chrono::high_resolution_clock> genesis = std::chrono::high_resolution_clock::now();

    class genesis_formatter_flag : public spdlog::custom_flag_formatter
    {
    public:
        void format(const spdlog::details::log_msg& msg, const std::tm& tm_time, spdlog::memory_buf_t& dest) override
        {
            const auto now = std::chrono::high_resolution_clock::now();
            const auto time_since_genesis = now - genesis;
            const auto seconds = std::chrono::duration_cast<std::chrono::duration<float, std::ratio<1, 1>>>(time_since_genesis).count();
            const auto str = std::to_string(seconds);
            dest.append(str.data(), str.data() + str.size());

        }

        std::unique_ptr<custom_flag_formatter> clone() const override
        {
            return spdlog::details::make_unique<genesis_formatter_flag>();
        }
    };

    struct ARGS_API impl {
        static std::shared_ptr<spdlog::logger> file_logger;
        static std::shared_ptr<spdlog::logger> console_logger;
        static std::unordered_map<std::thread::id, std::string> threadnames;
    };


    class thread_name_formatter_flag : public spdlog::custom_flag_formatter
    {
        void format(const spdlog::details::log_msg& msg, const std::tm& tm_time, spdlog::memory_buf_t& dest) override
        {
            std::string thread_ident;

            if (const auto it = impl::threadnames.find(std::this_thread::get_id()); it != impl::threadnames.end())
            {
                thread_ident = it->second;
            }
            else
            {
                thread_ident = std::to_string(args::core::force_value_cast<uint>(std::this_thread::get_id()));
            }

            dest.append(thread_ident.data(), thread_ident.data() + thread_ident.size());
        }
        std::unique_ptr<custom_flag_formatter> clone() const override
        {
            return spdlog::details::make_unique<thread_name_formatter_flag>();

        }
    };



#if defined(ARGS_KEEP_CONSOLE) || defined(ARGS_DEBUG)

#define LOG_FILE "stdout"
#define logger impl::console_logger

#else

#define LOG_FILE "logs/args-engine.log"
#define logger impl::file_logger

#endif


    inline void setup()
    {
        auto f = std::make_unique<spdlog::pattern_formatter>();

        f->add_flag<thread_name_formatter_flag>('f');
        f->add_flag<genesis_formatter_flag>('*');
        f->set_pattern("T+ %* [%^%=7l%$] [%=13!f] : %v");

        logger->set_formatter(std::move(f));
        //logger->set_pattern();
    }

    enum class severity
    {
        info,
        warn,
        error,
        debug
    };



    template <class... Args>
    void println(severity s, const char* format, Args&&... a)
    {
        switch (s)
        {
        case severity::info: logger->info(format, std::forward<Args>(a)...); break;
        case severity::warn: logger->warn(format, std::forward<Args>(a)...); break;
        case severity::error:logger->error(format, std::forward<Args>(a)...); break;
        case severity::debug:logger->debug(format, std::forward<Args>(a)...); break;
        default:break;
        }
    }


    inline void filter(severity level)
    {
        switch (level) {
        case severity::info: logger->set_level(spdlog::level::info); break;
        case severity::warn: logger->set_level(spdlog::level::warn); break;
        case severity::error: logger->set_level(spdlog::level::err); break;
        case severity::debug: logger->set_level(spdlog::level::debug); break;
        default:break;
        }
    }

    template <class... Args>
    void info(const char* format, Args&&...a)
    {
        println(severity::info, format, std::forward<Args>(a)...);
    }

    template <class... Args>
    void warn(const char* format, Args&&...a)
    {
        println(severity::warn, format, std::forward<Args>(a)...);
    }


    template <class... Args>
    void error(const char* format, Args&&...a)
    {
        println(severity::error, format, std::forward<Args>(a)...);
    }

    template <class... Args>
    void debug(const char* format, Args&&...a)
    {
        println(severity::debug, format, std::forward<Args>(a)...);
    }

}
#undef logger
