#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <cstdint>
#include <iostream>
#include <sstream>

#define LOGGER_STREAM(logger, lvl, stream)                                     \
    ({                                                                         \
        static_assert(                                                         \
            ::std::is_same<                                                    \
                typename std::remove_cv<typename std::remove_reference<        \
                    decltype(logger)>::type>::type,                            \
                typename ::ybot_ln::Logger>::value,                            \
            "First argument to logging macros must be an ybot_ln::Logger");    \
        static_assert(                                                         \
            ::std::is_same<decltype(lvl), typename ::ybot_ln::MSG_LVL>::value, \
            "Second argument to logging macros must be an ybot_ln::MSG_LVL");  \
        std::stringstream ss;                                                  \
        ss << stream;                                                          \
        logger(lvl, ss.str());                                                  \
    })

namespace ybot_ln
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
    void set_debug(const bool &n_debug);
    void set_verbose(const bool &n_verbose);
    void set_save_prm(const bool &n_save, const std::string n_save_path = "");

  private:
    Logger();
    Logger(const Logger &root) = delete;
    Logger &operator=(const Logger &) = delete;
    std::string get_time();

    bool verbose = false;
    bool debug = false;
    bool save = false;
    std::string save_path;
};

} // namespace ybot_ln

#endif // LOGGER_HPP