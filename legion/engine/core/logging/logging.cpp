#include <core/logging/logging.hpp>


namespace legion::core::log {
    cstring impl::log_file = "logs/legion-engine.log";
    std::shared_ptr<spdlog::logger> impl::file_logger;
    std::shared_ptr<spdlog::logger> impl::console_logger = spdlog::stdout_color_mt("does-not-matter");
    std::shared_ptr<spdlog::logger> impl::logger = impl::console_logger;
    async::rw_spinlock impl::thread_names_lock;
    std::unordered_map<std::thread::id, std::string> impl::thread_names;

}
