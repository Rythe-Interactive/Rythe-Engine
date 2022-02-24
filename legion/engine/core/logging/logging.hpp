#pragma once

#define SPDLOG_HEADER_ONLY
#include <sstream>
#include <thread>

#include <Optick/optick.h>

#if !defined(DOXY_EXCLUDE)
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/pattern_formatter.h>
#endif

#include <core/platform/platform.hpp>
#include <core/types/types.hpp>
#include <core/math/math.hpp>
#include <core/common/exception.hpp>
#include <core/async/rw_spinlock.hpp>

/** @file logging.hpp */
#if !defined(DOXY_EXCLUDE)
namespace fmt
{
    template <>
    struct formatter<std::thread::id>
    {

        constexpr const char* parse(format_parse_context& ctx)
        {
            auto it = ctx.begin(), end = ctx.end();

            if (!it)
                return nullptr;

            if (it != end && *it != '}')
                throw format_error("invalid format");
            return it++;
        }

        template <typename FormatContext>
        auto format(const std::thread::id& p, FormatContext& ctx) {
            std::ostringstream oss;
            oss << p;
            return format_to(ctx.out(), "{}", oss.str());
        }

    };

    template <>
    struct formatter<legion::core::exception>
    {

        constexpr const char* parse(format_parse_context& ctx)
        {
            auto it = ctx.begin(), end = ctx.end();

            if (!it)
                return nullptr;

            if (it != end && *it != '}')
                throw format_error("invalid format");
            return it++;
        }

        template <typename FormatContext>
        auto format(const legion::core::exception& error, FormatContext& ctx)
        {
            return format_to(ctx.out(), "[{}({}) T {}(...)] {}", error.file(), error.line(), error.func(), error.what());
        }
    };

    template <>
    struct formatter<legion::core::fs_error>
    {

        constexpr const char* parse(format_parse_context& ctx)
        {
            auto it = ctx.begin(), end = ctx.end();

            if (!it)
                return nullptr;

            if (it != end && *it != '}')
                throw format_error("invalid format");
            return it++;
        }

        template <typename FormatContext>
        auto format(const legion::core::fs_error& error, FormatContext& ctx)
        {
            return format_to(ctx.out(), "[{}({}) T {}(...)] {}", error.file(), error.line(), error.func(), error.what());
        }
    };

    template <>
    struct formatter<legion::core::math::float2> {
        // Presentation format: 'f' - fixed, 'e' - exponential.
        char presentation = 'f';

        // Parses format specifications of the form ['f' | 'e'].
        constexpr const char* parse(format_parse_context& ctx) {
            // auto parse(format_parse_context &ctx) -> decltype(ctx.begin()) // c++11
              // [ctx.begin(), ctx.end()) is a character range that contains a part of
              // the format string starting from the format specifications to be parsed,
              // e.g. in
              //
              //   fmt::format("{:f} - point of interest", point{1, 2});
              //
              // the range will contain "f} - point of interest". The formatter should
              // parse specifiers until '}' or the end of the range. In this example
              // the formatter should parse the 'f' specifier and return an iterator
              // pointing to '}'.

              // Parse the presentation format and store it in the formatter:
            auto it = ctx.begin(), end = ctx.end();

            if (!it)
                return nullptr;

            if (it != end && (*it == 'f' || *it == 'e')) presentation = *it++;

            // Check if reached the end of the range:
            if (it != end && *it != '}')
                throw format_error("invalid format");

            // Return an iterator past the end of the parsed range:
            return it;
        }

        // Formats the point p using the parsed format specification (presentation)
        // stored in this formatter.
        template <typename FormatContext>
        auto format(const legion::core::math::float2& p, FormatContext& ctx) {
            // auto format(const point &p, FormatContext &ctx) -> decltype(ctx.out()) // c++11
              // ctx.out() is an output iterator to write to.
            return format_to(
                ctx.out(),
                presentation == 'f' ? "({:.1f}, {:.1f})" : "({:.1e}, {:.1e})",
                p.x, p.y);
        }
    };

    template <>
    struct formatter<legion::core::math::int2> {

        constexpr const char* parse(format_parse_context& ctx) {
            auto it = ctx.begin(), end = ctx.end();

            if (!it)
                return nullptr;

            if (it != end && *it != '}')
                throw format_error("invalid format");
            return it++;
        }

        template <typename FormatContext>
        auto format(const legion::core::math::int2& p, FormatContext& ctx) {
            return format_to(
                ctx.out(),
                "({}, {})",
                p.x, p.y);
        }
    };

    template <>
    struct formatter<legion::core::math::float3> {
        char presentation = 'f';

        constexpr const char* parse(format_parse_context& ctx) {
            auto it = ctx.begin(), end = ctx.end();

            if (!it)
                return nullptr;

            if (it != end && (*it == 'f' || *it == 'e')) presentation = *it++;

            if (it != end && *it != '}')
                throw format_error("invalid format");

            return it;
        }

        template <typename FormatContext>
        auto format(const legion::core::math::float3& p, FormatContext& ctx) {
            return format_to(
                ctx.out(),
                presentation == 'f' ? "({:.1f}, {:.1f}, {:.1f})" : "({:.1e}, {:.1e}, {:.1e})",
                p.x, p.y, p.z);
        }
    };

    template <>
    struct formatter<legion::core::math::int3> {

        constexpr const char* parse(format_parse_context& ctx) {
            auto it = ctx.begin(), end = ctx.end();

            if (!it)
                return nullptr;

            if (it != end && *it != '}')
                throw format_error("invalid format");
            return it++;
        }

        template <typename FormatContext>
        auto format(const legion::core::math::int3& p, FormatContext& ctx) {
            return format_to(
                ctx.out(),
                "({}, {}, {})",
                p.x, p.y, p.z);
        }
    };

    template <>
    struct formatter<legion::core::math::float4> {
        char presentation = 'f';

        constexpr const char* parse(format_parse_context& ctx) {
            auto it = ctx.begin(), end = ctx.end();

            if (!it)
                return nullptr;

            if (it != end && (*it == 'f' || *it == 'e')) presentation = *it++;

            if (it != end && *it != '}')
                throw format_error("invalid format");

            return it;
        }

        template <typename FormatContext>
        auto format(const legion::core::math::float4& p, FormatContext& ctx) {
            return format_to(
                ctx.out(),
                presentation == 'f' ? "({:.1f}, {:.1f}, {:.1f}, {:.1f})" : "({:.1e}, {:.1e}, {:.1e}, {:.1e})",
                p.x, p.y, p.z, p.w);
        }
    };

    template <>
    struct formatter<legion::core::math::quat> {
        char presentation = 'f';

        constexpr const char* parse(format_parse_context& ctx) {
            auto it = ctx.begin(), end = ctx.end();

            if (!it)
                return nullptr;

            if (it != end && (*it == 'f' || *it == 'e')) presentation = *it++;

            if (it != end && *it != '}')
                throw format_error("invalid format");

            return it;
        }

        template <typename FormatContext>
        auto format(const legion::core::math::quat& p, FormatContext& ctx) {
            return format_to(
                ctx.out(),
                presentation == 'f' ? "(({:.1f}, {:.1f}, {:.1f}), {:.1f})" : "(({:.1e}, {:.1e}, {:.1e}), {:.1e})",
                p.i, p.j, p.k, p.w);
        }
    };

}
#endif


namespace legion::core::log
{
    using logger_ptr = std::shared_ptr<spdlog::logger>;

    /** @brief Holds the non const static data of logging. */
    struct impl
    {
        cstring logFile = "logs/legion-engine.log";
        logger_ptr logger;
        logger_ptr fileLogger;
        logger_ptr consoleLogger = spdlog::stdout_color_mt("console-logger");
        logger_ptr undecoratedLogger = spdlog::stdout_color_mt("undecorated-logger");
        async::rw_spinlock threadNamesLock;
        std::unordered_map<std::thread::id, std::string> threadNames;

        static impl& get();
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
        L_NODISCARD std::unique_ptr<custom_flag_formatter> clone() const override
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
            //std::string thread_ident;
            thread_local static std::string* thread_ident;

            if (!thread_ident)
            {
                auto& inst = impl::get();
                async::readonly_guard guard(inst.threadNamesLock);

                if (inst.threadNames.count(std::this_thread::get_id()))
                {
                    thread_ident = &inst.threadNames.at(std::this_thread::get_id());
                }
                else
                {
                    std::ostringstream oss;
                    oss << std::this_thread::get_id();
                    {
                        async::readwrite_guard wguard(inst.threadNamesLock);
                        thread_ident = &inst.threadNames[std::this_thread::get_id()];
                    }
                    *thread_ident = oss.str();

                    //NOTE(algo-ryth-mix): this conversion is not portable 
                    //thread_ident = std::to_string(legion::core::force_value_cast<uint>(std::this_thread::get_id()));
                }
            }

            dest.append(thread_ident->data(), thread_ident->data() + thread_ident->size());
        }
        std::unique_ptr<custom_flag_formatter> clone() const override
        {
            return spdlog::details::make_unique<thread_name_formatter_flag>();

        }
    };

    inline void initLogger(logger_ptr& logger)
    {
        auto f = std::make_unique<spdlog::pattern_formatter>();

        f->add_flag<thread_name_formatter_flag>('f');
        f->add_flag<genesis_formatter_flag>('*');
        f->set_pattern("T+ %* [%^%=7l%$] [%=13!f] : %v");

        logger->set_formatter(std::move(f));
    }


    inline void setLogger(const logger_ptr& newLogger)
    {
        impl::get().logger = newLogger;
    }

    /** @brief selects the severity you want to filter for or print with */
    enum class severity
    {
        trace,   // lowest severity
        debug,
        info,
        warn,
        error,
        fatal // highest severity
    };

    constexpr severity severity_trace = severity::trace;
    constexpr severity severity_debug = severity::debug;
    constexpr severity severity_info = severity::info;
    constexpr severity severity_warn = severity::warn;
    constexpr severity severity_error = severity::error;
    constexpr severity severity_fatal = severity::fatal;

    constexpr spdlog::level::level_enum args2spdlog(severity s)
    {
        switch (s)
        {
        case severity_trace:return spdlog::level::trace;
        case severity_debug:return spdlog::level::debug;
        case severity_info: return spdlog::level::info;
        case severity_warn: return spdlog::level::warn;
        case severity_error:return spdlog::level::err;
        case severity_fatal:return spdlog::level::critical;
        }
        return spdlog::level::err;
    }

    /** @brief prints a log line, using the specified `severity`
     *  @param s The severity you wan't to report this log with
     *  @param format The format string you want to print
     *  @param a The arguments to the format string
     *  @note This uses fmt lib style syntax check
     *         https://fmt.dev/latest/syntax.html
     */
    template <class... Args, class FormatString>
    void println(severity s, const FormatString& format, Args&&... a)
    {
        impl::get().logger->log(args2spdlog(s), format, std::forward<Args>(a)...);
    }

    /** @brief same as println but uses the undecorated logger */
    template <class... Args, class FormatString>
    void undecoratedln(severity s, const FormatString& format, Args&&... a)
    {
        impl::get().undecoratedLogger->log(args2spdlog(s), format, std::forward<Args>(a)...);
    }

    /** @brief prints a log line, using the specified `severity`
     *  @param level selects the severity level you are interested in
     */
    inline void filter(severity level)
    {
        auto& inst = impl::get();
        inst.logger->set_level(args2spdlog(level));
        inst.undecoratedLogger->set_level(args2spdlog(level));
    }

    /** @brief same as println but with severity = trace */
    template<class... Args, class FormatString>
    void undecoratedTrace(const FormatString& format, Args&&... a)
    {
        undecoratedln(severity::trace, format, std::forward<Args>(a)...);
    }

    /** @brief same as println but with severity = debug */
    template<class... Args, class FormatString>
    void undecoratedDebug(const FormatString& format, Args&&...a)
    {
        undecoratedln(severity::debug, format, std::forward<Args>(a)...);
    }

    /** @brief same as undecoratedln but with severity = info */
    template<class... Args, class FormatString>
    void undecoratedInfo(const FormatString& format, Args&&...a)
    {
        undecoratedln(severity::info, format, std::forward<Args>(a)...);
    }

    /** @brief same as undecoratedln but with severity = warn */
    template<class... Args, class FormatString>
    void undecoratedWarn(const FormatString& format, Args&&...a)
    {
        undecoratedln(severity::warn, format, std::forward<Args>(a)...);
    }

    /** @brief same as undecoratedln but with severity = error */
    template<class... Args, class FormatString>
    void undecoratedError(const FormatString& format, Args&&...a)
    {
        undecoratedln(severity::error, format, std::forward<Args>(a)...);
    }

    /** @brief same as undecoratedln but with severity = fatal */
    template<class... Args, class FormatString>
    void undecoratedFatal(const FormatString& format, Args&&...a)
    {
        undecoratedln(severity::fatal, format, std::forward<Args>(a)...);
    }

    /** @brief same as println but with severity = trace */
    template<class... Args, class FormatString>
    void trace(const FormatString& format, Args&&... a)
    {
        println(severity::trace, format, std::forward<Args>(a)...);
    }

    /** @brief same as println but with severity = debug */
    template<class... Args, class FormatString>
    void debug(const FormatString& format, Args&&...a)
    {
        println(severity::debug, format, std::forward<Args>(a)...);
    }

    /** @brief same as println but with severity = info */
    template<class... Args, class FormatString>
    void info(const FormatString& format, Args&&...a)
    {
        println(severity::info, format, std::forward<Args>(a)...);
    }

    /** @brief same as println but with severity = warn */
    template<class... Args, class FormatString>
    void warn(const FormatString& format, Args&&...a)
    {
        println(severity::warn, format, std::forward<Args>(a)...);
    }

    /** @brief same as println but with severity = error */
    template<class... Args, class FormatString>
    void error(const FormatString& format, Args&&...a)
    {
        println(severity::error, format, std::forward<Args>(a)...);
    }

    /** @brief same as println but with severity = fatal */
    template<class... Args, class FormatString>
    void fatal(const FormatString& format, Args&&...a)
    {
        println(severity::fatal, format, std::forward<Args>(a)...);
    }

    namespace detail
    {
        inline byte _setup_impl()
        {
            auto& inst = impl::get();

            auto f = std::make_unique<spdlog::pattern_formatter>();
            f->set_pattern("%v");
            inst.undecoratedLogger->set_formatter(std::move(f));

            inst.fileLogger = spdlog::rotating_logger_mt(inst.logFile, inst.logFile, 1'048'576, 5);
            initLogger(inst.consoleLogger);
            initLogger(inst.fileLogger);

#if defined(LEGION_KEEP_CONSOLE) || defined(LEGION_DEBUG)
            inst.logger = inst.consoleLogger;
#else
            inst.logger = inst.fileLogger;
#endif

#if defined(LEGION_LOG_TRACE)
            filter(severity::trace);
#elif defined(LEGION_LOG_DEBUG)
            filter(severity::debug);
#elif defined(LEGION_LOG_INFO)
            filter(severity::info);
#elif defined(LEGION_LOG_WARN)
            filter(severity::warn);
#elif defined(LEGION_LOG_ERROR)
            filter(severity::error);
#elif defined(LEGION_LOG_FATAL)
            filter(severity::fatal);
#elif defined(LEGION_DEBUG)
            filter(severity::debug);
#else
            filter(severity::info);
#endif

            undecoratedInfo("== Initializing Logger");
            return 0;
        }
    }

    /** @brief sets up logging (do not call, invoked by engine) */
    inline void setup()
    {
        static auto v = detail::_setup_impl();
    }
}
#undef logger
