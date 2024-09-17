#include "youbot_lidar_nav/utils/task_thread.hpp"
#include "youbot_lidar_nav/utils/logger.hpp"
#include <format>

namespace ybotln
{

TaskThread::TaskThread(std::string name) : task_name{task_name}
{
}

std::string TaskThread::get_name() const
{
    std::stringstream id;
    id << task_thread.get_id();
    if (task_name.empty())
    {
        return std::format("[{}]", id.str());
    }
    return std::format("\"{}\" [{}]", task_name, id.str());
}

void TaskThread::task_dec()
{
    running = true;
    stop_flag = false;
    LOGGER_STREAM(MSG_LVL::INFO,
                  "The task " << get_name() << " started...");
    try
    {
        task();
    }
    catch (const std::exception &e)
    {
        LOGGER_STREAM(MSG_LVL::ERROR,
                      "The task " << get_name()
                                << " terminated with exception: " << e.what());
    }
    LOGGER_STREAM(MSG_LVL::INFO,
                  "The task " << get_name() << " completed");
    running = false;
}

void TaskThread::start()
{
    if (running)
    {
        LOGGER_STREAM(MSG_LVL::WARN,
                      "The task " << get_name() << " is already running");
        return;
    }
    task_thread = std::thread{&TaskThread::task_dec, this};
}

void TaskThread::stop()
{
    if (!running)
    {
        LOGGER_STREAM(MSG_LVL::WARN, "The task is not running");
        return;
    }
    stop_flag = true;
}

void TaskThread::wait()
{
    if (!running)
    {
        LOGGER_STREAM(MSG_LVL::WARN, "The task is not running");
        return;
    }
    task_thread.join();
}

TaskThread::~TaskThread()
{
    stop();
    wait();
}

} // namespace ybotln
