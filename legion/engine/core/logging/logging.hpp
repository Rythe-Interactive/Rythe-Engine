#pragma once

#define SPDLOG_HEADER_ONLY
#include <sstream>

#include <Optick/optick.h>

#if !defined(DOXY_EXCLUDE)
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/pattern_formatter.h>
#endif

#include <core/platform/platform.hpp>
#include <thread>
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

        constexpr auto parse(format_parse_context& ctx)
        {
            auto it = ctx.begin(), end = ctx.end();

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

        constexpr auto parse(format_parse_context& ctx)
        {
            auto it = ctx.begin(), end = ctx.end();

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

        constexpr auto parse(format_parse_context& ctx)
        {
            auto it = ctx.begin(), end = ctx.end();

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
    struct formatter<legion::core::math::vec2> {
        // Presentation format: 'f' - fixed, 'e' - exponential.
        char presentation = 'f';

        // Parses format specifications of the form ['f' | 'e'].
        constexpr auto parse(format_parse_context& ctx) {
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
        auto format(const legion::core::math::vec2& p, FormatContext& ctx) {
            // auto format(const point &p, FormatContext &ctx) -> decltype(ctx.out()) // c++11
              // ctx.out() is an output iterator to write to.
            return format_to(
                ctx.out(),
                presentation == 'f' ? "({:.1f}, {:.1f})" : "({:.1e}, {:.1e})",
                p.x, p.y);
        }
    };

    template <>
    struct formatter<legion::core::math::ivec2> {

        constexpr auto parse(format_parse_context& ctx) {
            auto it = ctx.begin(), end = ctx.end();

            if (it != end && *it != '}')
                throw format_error("invalid format");
            return it++;
        }

        template <typename FormatContext>
        auto format(const legion::core::math::ivec2& p, FormatContext& ctx) {
            return format_to(
                ctx.out(),
                "({}, {})",
                p.x, p.y);
        }
    };

    template <>
    struct formatter<legion::core::math::vec3> {
        char presentation = 'f';

        constexpr auto parse(format_parse_context& ctx) {
            auto it = ctx.begin(), end = ctx.end();
            if (it != end && (*it == 'f' || *it == 'e')) presentation = *it++;

            if (it != end && *it != '}')
                throw format_error("invalid format");

            return it;
        }

        template <typename FormatContext>
        auto format(const legion::core::math::vec3& p, FormatContext& ctx) {
            return format_to(
                ctx.out(),
                presentation == 'f' ? "({:.1f}, {:.1f}, {:.1f})" : "({:.1e}, {:.1e}, {:.1e})",
                p.x, p.y, p.z);
        }
    };

    template <>
    struct formatter<legion::core::math::ivec3> {

        constexpr auto parse(format_parse_context& ctx) {
            auto it = ctx.begin(), end = ctx.end();

            if (it != end && *it != '}')
                throw format_error("invalid format");
            return it++;
        }

        template <typename FormatContext>
        auto format(const legion::core::math::ivec3& p, FormatContext& ctx) {
            return format_to(
                ctx.out(),
                "({}, {}, {})",
                p.x, p.y, p.z);
        }
    };

    template <>
    struct formatter<legion::core::math::vec4> {
        char presentation = 'f';

        constexpr auto parse(format_parse_context& ctx) {
            auto it = ctx.begin(), end = ctx.end();
            if (it != end && (*it == 'f' || *it == 'e')) presentation = *it++;

            if (it != end && *it != '}')
                throw format_error("invalid format");

            return it;
        }

        template <typename FormatContext>
        auto format(const legion::core::math::vec4& p, FormatContext& ctx) {
            return format_to(
                ctx.out(),
                presentation == 'f' ? "({:.1f}, {:.1f}, {:.1f}, {:.1f})" : "({:.1e}, {:.1e}, {:.1e}, {:.1e})",
                p.x, p.y, p.z, p.w);
        }
    };

    template <>
    struct formatter<legion::core::math::color> {
        char presentation = 'f';

        constexpr auto parse(format_parse_context& ctx) {
            auto it = ctx.begin(), end = ctx.end();
            if (it != end && (*it == 'f' || *it == 'e')) presentation = *it++;

            if (it != end && *it != '}')
                throw format_error("invalid format");

            return it;
        }

        template <typename FormatContext>
        auto format(const legion::core::math::color& p, FormatContext& ctx) {
            return format_to(
                ctx.out(),
                presentation == 'f' ? "({:.1f}, {:.1f}, {:.1f}, {:.1f})" : "({:.1e}, {:.1e}, {:.1e}, {:.1e})",
                p.r, p.g, p.b, p.a);
        }
    };

    template <>
    struct formatter<legion::core::math::quat> {
        char presentation = 'f';

        constexpr auto parse(format_parse_context& ctx) {
            auto it = ctx.begin(), end = ctx.end();
            if (it != end && (*it == 'f' || *it == 'e')) presentation = *it++;

            if (it != end && *it != '}')
                throw format_error("invalid format");

            return it;
        }

        template <typename FormatContext>
        auto format(const legion::core::math::quat& p, FormatContext& ctx) {
            return format_to(
                ctx.out(),
                presentation == 'f' ? "(({:.1f}, {:.1f}, {:.1f}),r: {:.1f})" : "(({:.1e}, {:.1e}, {:.1e}),r: {:.1e})",
                p.x, p.y, p.z, p.w);
        }
    };

}
#endif


namespace legion::core::log
{
    using logger_ptr = std::shared_ptr<spdlog::logger>;

    /** @brief Holds the non const static data of logging. */
    struct impl {
        static cstring logFile;
        static logger_ptr logger;
        static logger_ptr fileLogger;
        static logger_ptr consoleLogger;
        static logger_ptr undecoratedLogger;
        static async::rw_spinlock threadNamesLock;
        static std::unordered_map<std::thread::id, std::string> threadNames;
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
                async::readonly_guard guard(impl::threadNamesLock);

                if (impl::threadNames.count(std::this_thread::get_id()))
                {
                    thread_ident = &impl::threadNames.at(std::this_thread::get_id());
                }
                else
                {
                    std::ostringstream oss;
                    oss << std::this_thread::get_id();
                    {
                        async::readwrite_guard wguard(impl::threadNamesLock);
                        thread_ident = &impl::threadNames[std::this_thread::get_id()];
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

    inline void initLogger(std::shared_ptr<spdlog::logger>& logger)
    {
        auto f = std::make_unique<spdlog::pattern_formatter>();

        f->add_flag<thread_name_formatter_flag>('f');
        f->add_flag<genesis_formatter_flag>('*');
        f->set_pattern("T+ %* [%^%=7l%$] [%=13!f] : %v");

        logger->set_formatter(std::move(f));
    }

    inline static logger_ptr& logger = impl::logger;
    inline static logger_ptr& consoleLogger = impl::consoleLogger;
    inline static logger_ptr& fileLogger = impl::fileLogger;
    inline static logger_ptr& undecoratedLogger = impl::undecoratedLogger;

    inline void setLogger(const logger_ptr& newLogger)
    {
        logger = newLogger;
    }

    /** @brief sets up logging (do not call, invoked by engine) */
    inline void setup()
    {
        auto f = std::make_unique<spdlog::pattern_formatter>();
        f->set_pattern("%v");
        impl::undecoratedLogger->set_formatter(std::move(f));

        impl::fileLogger = spdlog::rotating_logger_mt(impl::logFile, impl::logFile, 1'048'576, 5);
        initLogger(impl::consoleLogger);
        initLogger(impl::fileLogger);

#if defined(LEGION_KEEP_CONSOLE) || defined(LEGION_DEBUG)
        logger = impl::consoleLogger;
#else
        logger = impl::fileLogger;
#endif
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
        logger->log(args2spdlog(s), format, std::forward<Args>(a)...);
    }


    /** @brief prints a log line, using the specified `severity`
     *  @param level selects the severity level you are interested in
     */
    inline void filter(severity level)
    {
        logger->set_level(args2spdlog(level));
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

}
#undef logger
