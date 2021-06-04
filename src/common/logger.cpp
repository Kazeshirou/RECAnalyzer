#include "logger.hpp"

#include <chrono>
#include <iostream>

namespace my_log {

void Logger::debug(const std::string& system, const std::string& msg) {
    inner_log(time(), "DEBUG", system, msg);
}
void Logger::info(const std::string& system, const std::string& msg) {
    inner_log(time(), "INFO", system, msg);
}
void Logger::warning(const std::string& system, const std::string& msg) {
    inner_log(time(), "WARNING", system, msg);
}
void Logger::critical(const std::string& system, const std::string& msg) {
    inner_log(time(), "CRITICAL", system, msg);
}

void Logger::inner_log(size_t time, const std::string& level,
                       const std::string& system, const std::string& msg) {
    std::cerr << time << " | " << level << " | " << system << " | " << msg
              << std::endl;
    std::cerr.flush();
}

size_t Logger::time() {
    using namespace std::chrono;
    milliseconds ms =
        duration_cast<milliseconds>(system_clock::now().time_since_epoch());
    return ms.count();
}

}  // namespace my_log