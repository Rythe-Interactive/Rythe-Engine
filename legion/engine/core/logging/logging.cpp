#include <core/logging/logging.hpp>


namespace legion::core::log {
    cstring impl::logFile = "logs/legion-engine.log";
    logger_ptr impl::fileLogger;
    logger_ptr impl::consoleLogger = spdlog::stdout_color_mt("console-logger");
    logger_ptr impl::undecoratedLogger = spdlog::stdout_color_mt("undecorated-logger");
    logger_ptr impl::logger = impl::consoleLogger;
    async::rw_spinlock impl::threadNamesLock;
    std::unordered_map<std::thread::id, std::string> impl::threadNames;

}
