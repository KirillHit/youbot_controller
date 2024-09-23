#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <cstdint>
#include <iostream>
#include <sstream>

#define LOGGER_STREAM(lvl, stream)                                                                 \
    ({                                                                                             \
        static_assert(::std::is_same<decltype(lvl), typename ::ybotln::MSG_LVL>::value,            \
                      "First argument to logging macros must be an ybotln::MSG_LVL");              \
        std::stringstream ss;                                                                      \
        ss << stream;                                                                              \
        Logger::get_logger()(lvl, ss.str());                                                       \
    })

namespace ybotln
{

enum class MSG_LVL : uint8_t
{
    INFO,
    DEBUG,
    WARN,
    ERROR,
    FATAL
};

class Logger
{
  public:
    static Logger &get_logger();
    void operator()(const MSG_LVL &lvl, const std::string &str);
    void update_parameters();

  private:
    Logger();
    Logger(const Logger &root) = delete;
    Logger &operator=(const Logger &) = delete;
    std::string get_time();

    bool debug = false;
    bool save = false;
    std::string save_path;
};

} // namespace ybotln

#endif // LOGGER_HPP