#include <core/logging/logging.hpp>


namespace legion::core::log {
    cstring impl::log_file = nullptr;
    cstring impl::file_file = "logs/legion-engine.log";
    cstring impl::console_file = "stdout";
    std::shared_ptr<spdlog::logger> impl::logger;
    std::shared_ptr<spdlog::logger> impl::file_logger;
    std::shared_ptr<spdlog::logger> impl::console_logger = spdlog::stdout_color_mt("does-not-matter");
    std::unordered_map<std::thread::id, std::string> impl::thread_names;

}
