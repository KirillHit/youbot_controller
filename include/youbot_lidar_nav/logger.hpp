#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <cstdint>
#include <iostream>

namespace youbot_nav
{

enum class MSG_LVL : uint8_t
{
    INFO,
    WARN,
    ERROR,
    FATAL
};

class Logger
{
  public:
    static Logger &get_logger();
    void operator()(const MSG_LVL &lvl, std::ostringstream &ss);

  private:
    Logger();
    Logger(const Logger &root) = delete;
    Logger &operator=(const Logger &) = delete;

    bool verbose = false;
    bool debug = false;
    bool save = false;
    std::string save_path;
};

} // namespace youbot_nav

#endif // LOGGER_HPP