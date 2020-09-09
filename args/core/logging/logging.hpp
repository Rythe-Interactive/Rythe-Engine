#pragma once

#define SPDLOG_HEADER_ONLY
#include <sstream>
#include <core/logging/spdlog/spdlog.h>
#include <core/logging/spdlog/sinks/stdout_color_sinks.h>
#include <core/logging/spdlog/sinks/rotating_file_sink.h>
#include <core/logging/spdlog/pattern_formatter.h>
#include <core/types/type_util.hpp>

/** @file logging.hpp */

namespace args::core::log
{
    /** @brief Holds the non const static data of logging. */
    struct ARGS_API impl {
        static std::shared_ptr<spdlog::logger> file_logger;
        static std::shared_ptr<spdlog::logger> console_logger;
        static std::unordered_map<std::thread::id, std::string> thread_names;
    };


    /** @brief the time point at which the engine started */
    const static inline std::chrono::time_point<std::chrono::high_resolution_clock> genesis = std::chrono::high_resolution_clock::now();

    /** @class genesis_formatter_flag
     *  @brief Custom formatter flag that prints the time since the engine started in seconds.milliseconds
     */
    class genesis_formatter_flag : public spdlog::custom_flag_formatter
    {
    public:
        void format(const spdlog::details::log_msg& msg, const std::tm& tm_time, spdlog::memory_buf_t& dest) override
        {
            //get seconds since engine start
            const auto now = std::chrono::high_resolution_clock::now();
            const auto time_since_genesis = now - genesis;
            const auto seconds = std::chrono::duration_cast<std::chrono::duration<float, std::ratio<1, 1>>>(time_since_genesis).count();

            //convert to "--s.ms---"
            const auto str = std::to_string(seconds);

            //append to data
            dest.append(str.data(), str.data() + str.size());

        }

        //generates a new formatter flag
        A_NODISCARD std::unique_ptr<custom_flag_formatter> clone() const override
        {
            return spdlog::details::make_unique<genesis_formatter_flag>();
        }
    };

    /** @class thread_name_formatter_flag
     *  @brief Prints the name of the thread (if available) and otherwise the the TID.
     */
    class thread_name_formatter_flag : public spdlog::custom_flag_formatter
    {
        void format(const spdlog::details::log_msg& msg, const std::tm& tm_time, spdlog::memory_buf_t& dest) override
        {
            std::string thread_ident;

            if (const auto it = impl::thread_names.find(std::this_thread::get_id()); it != impl::thread_names.end())
            {
                thread_ident = it->second;
            }
            else
            {
                std::ostringstream oss;
                oss << std::this_thread::get_id();
                thread_ident = oss.str();

                //NOTE(algo-ryth-mix): this conversion is not portable 
                //thread_ident = std::to_string(args::core::force_value_cast<uint>(std::this_thread::get_id()));
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

    /** @brief sets up logging (do not call, invoked by engine) */
    inline void setup()
    {
        auto f = std::make_unique<spdlog::pattern_formatter>();

        f->add_flag<thread_name_formatter_flag>('f');
        f->add_flag<genesis_formatter_flag>('*');
        f->set_pattern("T+ %* [%^%=7l%$] [%=13!f] : %v");

        logger->set_formatter(std::move(f));
    }

    /** @brief selects the severity you want to filter for or print with */
    enum class severity
    {
        debug, // lowest severity
        info,
        warn,
        error  // highest severity
    };


    /** @brief prints a log line, using the specified `severity`
     *  @param s The severity you wan't to report this log with
     *  @param format The format string you want to print
     *  @param a The arguments to the format string
     *  @note This uses fmt lib style syntax check
     *         https://fmt.dev/latest/syntax.html
     */
    template <class... Args>
    void println(severity s, const char* format, Args&&... a)
    {
        switch (s)
        {
        case severity::debug:logger->debug(format, std::forward<Args>(a)...); break;
        case severity::info: logger->info(format, std::forward<Args>(a)...); break;
        case severity::warn: logger->warn(format, std::forward<Args>(a)...); break;
        case severity::error:logger->error(format, std::forward<Args>(a)...); break;
        default:break;
        }
    }


    /** @brief prints a log line, using the specified `severity`
     *  @param level selects the severity level you are interested in
     */
    inline void filter(severity level)
    {
        switch (level) {
        case severity::debug: logger->set_level(spdlog::level::debug); break;
        case severity::info: logger->set_level(spdlog::level::info); break;
        case severity::warn: logger->set_level(spdlog::level::warn); break;
        case severity::error: logger->set_level(spdlog::level::err); break;
        default:break;
        }
    }

    /** @brief same as println but with severity = debug */
    template <class... Args>
    void debug(const char* format, Args&&...a)
    {
        println(severity::debug, format, std::forward<Args>(a)...);
    }

    /** @brief same as println but with severity = info */
    template <class... Args>
    void info(const char* format, Args&&...a)
    {
        println(severity::info, format, std::forward<Args>(a)...);
    }

    /** @brief same as println but with severity = warn */
    template <class... Args>
    void warn(const char* format, Args&&...a)
    {
        println(severity::warn, format, std::forward<Args>(a)...);
    }

    /** @brief same as println but with severity = error */
    template <class... Args>
    void error(const char* format, Args&&...a)
    {
        println(severity::error, format, std::forward<Args>(a)...);
    }



}
#undef logger
