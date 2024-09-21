#include "youbot_lidar_nav/utils/task_thread.hpp"
#include "youbot_lidar_nav/utils/logger.hpp"
#include <format>

namespace ybotln
{

Task::Task(std::string name) : task_name{name} {}

std::string Task::get_name() const
{
    return task_name;
}

std::string Task::log_name() const
{
    std::stringstream id;
    id << task_thread.get_id();
    return std::format("\"{}\" [{}]", task_name, id.str());
}

void Task::set_task_poll(TaskPool *n_pool)
{
    task_pool = n_pool;
}

void Task::task_dec()
{
    //stop_flag = false;
    LOGGER_STREAM(MSG_LVL::INFO, "The task " << log_name() << " started...");
    try
    {
        task();
    }
    catch (const std::exception &e)
    {
        LOGGER_STREAM(MSG_LVL::ERROR,
                      "The task " << log_name() << " terminated with exception: " << e.what());
    }
    LOGGER_STREAM(MSG_LVL::INFO, "The task " << log_name() << " completed");
}

void Task::start()
{
    if (running)
    {
        LOGGER_STREAM(MSG_LVL::DEBUG, "The task " << log_name() << " is already running");
        return;
    }
    stop_flag = false;
    task_thread = std::thread{&Task::task_dec, this};
    running = true;
}

void Task::stop()
{
    if (!running)
    {
        LOGGER_STREAM(MSG_LVL::DEBUG, "The task " << log_name() << "is not running");
        return;
    }
    stop_flag = true;
    join();
    running = false;
}

void Task::join()
{
    if (!task_thread.joinable())
    {
        return;
    }
    task_thread.join();
}

Task::~Task()
{
    stop();
}

void TaskPool::start(std::string name)
{
    std::lock_guard<std::mutex> lock(tasks_lock);
    try
    {
        tasks.at(name).first->start();
    }
    catch (const std::out_of_range &e)
    {
        LOGGER_STREAM(MSG_LVL::WARN, "Attempt to access a non-existent task: " << name);
    }
}

void TaskPool::stop(std::string name)
{
    std::lock_guard<std::mutex> lock(tasks_lock);
    try
    {
        tasks.at(name).first->stop();
    }
    catch (const std::out_of_range &e)
    {
        LOGGER_STREAM(MSG_LVL::WARN, "Attempt to access a non-existent task: " << name);
    }
}

void TaskPool::start_all()
{
    std::lock_guard<std::mutex> lock(tasks_lock);
    for (auto const &[key, val] : tasks)
    {
        val.first->start();
    }
}

void TaskPool::stop_all()
{
    std::lock_guard<std::mutex> lock(tasks_lock);
    for (auto const &[key, val] : tasks)
    {
        val.first->stop();
    }
}

void TaskPool::add_task(std::unique_ptr<Task> &&task)
{
    std::lock_guard<std::mutex> lock(tasks_lock);
    std::string name = task->get_name();
    if (tasks.contains(name))
    {
        LOGGER_STREAM(MSG_LVL::ERROR, "This thread already exists! The task was not added");
        return;
    }
    task->set_task_poll(this);
    tasks[name].first = std::move(task);
}

void TaskPool::add_command(std::string name, std::shared_ptr<Command> command)
{
    std::lock_guard<std::mutex> lock(tasks_lock);
    try
    {
        tasks.at(name).second.push(command);
    }
    catch (const std::out_of_range &e)
    {
        LOGGER_STREAM(MSG_LVL::WARN, "Attempt to access a non-existent task: " << name);
    }
}

std::queue<std::shared_ptr<Command>> TaskPool::get_commands(std::string name)
{
    std::lock_guard<std::mutex> lock(tasks_lock);
    std::queue<std::shared_ptr<Command>> empty;
    try
    {
        std::swap(tasks.at(name).second, empty);
    }
    catch (const std::out_of_range &e)
    {
        LOGGER_STREAM(MSG_LVL::WARN, "Attempt to access a non-existent task: " << name);
    }
    return empty;
}

void Task::emit_command(std::string name, std::shared_ptr<Command> command)
{
    if (task_pool == nullptr)
    {
        LOGGER_STREAM(MSG_LVL::WARN, "The task does not belong to the pool. Command not called.");
        return;
    }
    task_pool->add_command(name, command);
}

void Task::process_commands()
{
    if (task_pool == nullptr)
    {
        LOGGER_STREAM(MSG_LVL::WARN, "The task does not belong to the pool. Command not called.");
        return;
    }
    auto task_queue = task_pool->get_commands(task_name);
    while (!task_queue.empty())
    {
        task_queue.front()->execute(*this);
        task_queue.pop();
    }
}

} // namespace ybotln
