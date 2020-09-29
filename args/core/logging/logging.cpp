#include <core/logging/logging.hpp>


namespace args::core::log {

std::shared_ptr<spdlog::logger> impl::file_logger  = spdlog::rotating_logger_mt(LOG_FILE,LOG_FILE,1'048'576,5);
std::shared_ptr<spdlog::logger> impl::console_logger = spdlog::stdout_color_mt("does-not-matter");
std::unordered_map<std::thread::id, std::string> impl::thread_names;

}
