#pragma once

#include <string>

namespace log {

class Logger {
public:
    static void debug(const std::string& system, const std::string& msg);
    static void warning(const std::string& system, const std::string& msg);
    static void critical(const std::string& system, const std::string& msg);

protected:
    static void inner_log(size_t time, const std::string& level,
                          const std::string& system, const std::string& msg);

    static size_t time();
};

}  // namespace log