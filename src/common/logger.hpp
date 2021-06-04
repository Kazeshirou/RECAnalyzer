#pragma once

#include <string>

namespace my_log {

class Logger {
public:
    static void debug(const std::string& system, const std::string& msg);
    static void info(const std::string& system, const std::string& msg);
    static void warning(const std::string& system, const std::string& msg);
    static void critical(const std::string& system, const std::string& msg);

    static size_t time();

protected:
    static void inner_log(size_t time, const std::string& level,
                          const std::string& system, const std::string& msg);
};

}  // namespace my_log