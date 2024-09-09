#include <chrono>
#include <format>
#include <string>

#include "youbot_lidar_nav/logger.hpp"

namespace ybot_ln
{

Logger &Logger::get_logger()
{
    static Logger single_instance;
    return single_instance;
}

Logger::Logger()
{
}

std::string Logger::get_time()
{
    using namespace std::chrono;
    auto now = system_clock::now();
    auto ms = duration_cast<milliseconds>(now.time_since_epoch()) % 1000;

    auto timer = system_clock::to_time_t(now);
    std::tm bt = *std::localtime(&timer);

    std::ostringstream oss;
    oss << std::put_time(&bt, "%H:%M:%S");
    oss << '.' << std::setfill('0') << std::setw(3) << ms.count();
    return oss.str();
}

void Logger::operator()(const MSG_LVL &lvl, const std::string &str)
{
    if ((lvl == MSG_LVL::DEBUG) && !debug)
    {
        return;
    }

    if (!verbose && !save)
    {
        return;
    }

    constexpr std::string_view msg_lvl[] = {"INFO", "DEBUG", "WARN", "ERROR",
                                            "FATAL"};

    std::string log_str = std::format("[{}][{}]: {}", get_time(),
                                      msg_lvl[static_cast<size_t>(lvl)], str);

    if (verbose)
    {
        std::cout << log_str << std::endl;
    }

    if (save)
    {
        // TODO
    }
}

void Logger::set_debug(const bool &n_debug)
{
    debug = n_debug;
}

void Logger::set_verbose(const bool &n_verbose)
{
    verbose = n_verbose;
}

void Logger::set_save_prm(const bool &n_save, const std::string n_save_path)
{
    // TODO
}

} // namespace ybot_ln