#include "youbot_lidar_nav/utils/logger.hpp"

#include "youbot_lidar_nav/utils/parameter_server.hpp"

#include <chrono>
#include <filesystem>
#include <format>
#include <string>

#define RESET "\033[0m"
#define BLACK "\033[30m"
#define RED "\033[31m"
#define GREEN "\033[32m"
#define YELLOW "\033[33m"
#define BLUE "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN "\033[36m"
#define WHITE "\033[37m"

namespace ybotln {

Logger& Logger::get_logger()
{
    static Logger single_instance;
    return single_instance;
}

void Logger::set_debug(bool flag)
{
    debug = flag;
}

void Logger::set_save(bool flag)
{
    if (!flag)
    {
        if (out_file.is_open())
            out_file.close();
        save = false;
        return;
    }

    std::filesystem::create_directories("log");
    std::string file = "log/" + std::format("{0:%F_%R}", std::chrono::system_clock::now()) + ".log";
    out_file.open(file);
    if (!out_file.is_open())
    {
        out_file.close();
        this->operator()(MSG_LVL::ERROR, "Failed to open file: " + file + " !");
        return;
    }

    save = true;
    this->operator()(MSG_LVL::INFO, "Recording logs to file: " + file);
}

void Logger::operator()(const MSG_LVL& lvl, const std::string& str)
{
    if ((lvl == MSG_LVL::DEBUG) && !debug)
    {
        return;
    }

    constexpr std::string_view msg_lvl[] = {"INFO ", "DEBUG", "WARN ", "ERROR", "FATAL"};
    constexpr std::string_view msg_clr[] = {RESET, CYAN, YELLOW, RED, RED};

    std::string log_str =
        std::format("[{}][{}]: {}", get_time(), msg_lvl[static_cast<size_t>(lvl)], str);

    std::cout << msg_clr[static_cast<size_t>(lvl)] << log_str << RESET << std::endl;

    if (save)
    {
        out_file << log_str << std::endl;
    }
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

}  // namespace ybotln
