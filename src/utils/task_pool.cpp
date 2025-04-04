#include "youbot_lidar_nav/utils/task_pool.hpp"

#include "youbot_lidar_nav/utils/logger.hpp"

#include <format>
#include <utility>

namespace ybotln {

/**************************** Task class ****************************/

Task::Task(std::string name) : task_name {name}, command_smph {0} {}

Task::~Task()
{
    stop();
    release_commands();
    join();
}

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

void Task::set_task_poll(TaskPool* n_pool)
{
    task_pool = n_pool;
}

void Task::task_dec()
{
    LOGGER_STREAM(MSG_LVL::INFO, "The task " << log_name() << " started...");
    try
    {
        task();
    }
    catch (const std::exception& e)
    {
        LOGGER_STREAM(MSG_LVL::ERROR,
                      "The task " << log_name() << " terminated with exception: " << e.what());
    }
    release_commands();
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
    task_thread = std::thread {&Task::task_dec, this};
    running = true;
}

void Task::stop()
{
    stop_flag = true;
    command_smph.release();
}

void Task::join()
{
    if (!task_thread.joinable())
    {
        return;
    }
    task_thread.join();
    running = false;
}

void Task::add_command(std::shared_ptr<Command> command)
{
    std::lock_guard<std::mutex> lock(cmd_lock);
    commands.push(command);
    command_smph.release();
}

void Task::emit_command(std::string name, std::shared_ptr<Command> command)
{
    if (task_pool == nullptr)
    {
        LOGGER_STREAM(MSG_LVL::WARN, "The task does not belong to the pool. Command not called.");
        return;
    }
    task_pool->add_command_to(name, command);
}

void Task::wait_process_commands()
{
    if (stop_flag)
    {
        return;
    }
    command_smph.acquire();
    process_commands();
}

bool Task::try_process_commands()
{
    if (stop_flag)
    {
        return false;
    }
    if (command_smph.try_acquire())
    {
        process_commands();
        return true;
    }
    return false;
}

bool Task::try_process_commands_for(const std::chrono::milliseconds& rel_time)
{
    if (stop_flag)
    {
        return false;
    }
    if (command_smph.try_acquire_for(rel_time))
    {
        process_commands();
        return true;
    }
    return false;
}

template <typename _Clock, typename _Dur>
bool Task::try_process_commands_until(const std::chrono::time_point<_Clock, _Dur>& time_point)
{
    if (stop_flag)
    {
        return false;
    }
    if (command_smph.try_acquire_until(time_point))
    {
        process_commands();
        return true;
    }
    return false;
}
template bool Task::try_process_commands_until(
    const std::chrono::time_point<std::chrono::steady_clock, std::chrono::steady_clock::duration>&
        time_point);
template bool Task::try_process_commands_until(
    const std::chrono::time_point<std::chrono::system_clock, std::chrono::system_clock::duration>&
        time_point);

void Task::process_commands()
{
    std::unique_lock<std::mutex> lock(cmd_lock);
    std::queue<std::shared_ptr<Command>> exe_cmds;
    commands.swap(exe_cmds);
    lock.unlock();
    for (; !exe_cmds.empty(); exe_cmds.pop())
    {
        exe_cmds.front()->execute_dec(*this);
    }
}

void Task::release_commands()
{
    std::lock_guard<std::mutex> lock(cmd_lock);
    for (; !commands.empty(); commands.pop())
    {
        commands.front()->release();
    }
}

/**************************** TaskPool class ****************************/

void TaskPool::start(const std::string name)
{
    std::shared_lock<std::shared_mutex> lock(tasks_lock);
    if (!tasks.contains("name"))
    {
        LOGGER_STREAM(MSG_LVL::WARN, "Attempt to access a non-existent task: " << name);
        return;
    }
    tasks.at(name)->start();
}

void TaskPool::stop(const std::string name)
{
    std::shared_lock<std::shared_mutex> lock(tasks_lock);
    if (!tasks.contains("name"))
    {
        LOGGER_STREAM(MSG_LVL::WARN, "Attempt to access a non-existent task: " << name);
        return;
    }
    tasks.at(name)->stop();
    tasks.at(name)->join();
}

void TaskPool::start_all()
{
    std::shared_lock<std::shared_mutex> lock(tasks_lock);
    for (auto const& [key, val] : tasks)
    {
        val->start();
    }
}

void TaskPool::stop_all()
{
    std::shared_lock<std::shared_mutex> lock(tasks_lock);
    for (auto const& [key, val] : tasks)
    {
        val->stop();
    }
    for (auto const& [key, val] : tasks)
    {
        val->join();
    }
}

void TaskPool::add_task(std::unique_ptr<Task>&& task)
{
    std::unique_lock<std::shared_mutex> lock(tasks_lock);
    std::string name = task->get_name();
    if (tasks.contains(name))
    {
        LOGGER_STREAM(MSG_LVL::ERROR, "This thread already exists! The task was not added");
        return;
    }
    task->set_task_poll(this);
    tasks[name] = std::move(task);
}

void TaskPool::add_command_to(std::string name, std::shared_ptr<Command> command)
{
    std::shared_lock<std::shared_mutex> lock(tasks_lock);
    if (!tasks.contains(name))
    {
        LOGGER_STREAM(MSG_LVL::WARN, "Attempt to access a non-existent task: " << name);
        command->release();
        return;
    }
    tasks.at(name)->add_command(command);
}

/**************************** Command interface ****************************/

Command::Command() : completion_smph {0} {}

void Command::execute_dec(Task& task)
{
    try
    {
        execute(task);
    }
    catch (const std::bad_cast& e)
    {
        LOGGER_STREAM(MSG_LVL::ERROR, "The task received an unexpected command!");
    }
    release();
}

void Command::wait_command()
{
    completion_smph.acquire();
}

bool Command::try_command()
{
    return completion_smph.try_acquire();
}

bool Command::try_command_for(const std::chrono::milliseconds& rel_time)
{
    return completion_smph.try_acquire_for(rel_time);
}

void Command::release()
{
    completion_smph.release();
}

bool Command::result() const
{
    return result_;
}

}  // namespace ybotln
