#ifndef THREAD_POOL_HPP
#define THREAD_POOL_HPP

#include <atomic>
#include <chrono>
#include <map>
#include <memory>
#include <mutex>
#include <queue>
#include <semaphore>
#include <shared_mutex>
#include <string>
#include <thread>
#include <utility>

namespace ybotln
{

class TaskPool;
class Command;

class Task
{
  public:
    Task(std::string name);
    virtual ~Task();
    // Functions intended for TaskPool or other owner
    void start();
    void stop();
    void join();
    std::string get_name() const;
    void set_task_poll(TaskPool *n_pool);
    void add_command(std::shared_ptr<Command> command);

  protected:
    // Main code of the child task
    virtual void task() = 0;
    // Command interface for child classes
    void emit_command(std::string name, std::shared_ptr<Command> command);
    void wait_process_commands();
    bool try_process_commands();
    bool try_process_commands_for(const std::chrono::milliseconds &rel_time);
    bool try_process_commands_until(
        const std::chrono::time_point<std::chrono::steady_clock> &time_point);
    std::atomic<bool> stop_flag;

  private:
    void task_dec();
    std::string log_name() const;
    void process_commands();
    std::string task_name;
    std::thread task_thread;
    std::atomic<bool> running;
    TaskPool *task_pool = nullptr;
    std::queue<std::shared_ptr<Command>> commands;
    std::mutex cmd_lock;                // Protecting access to commands
    std::binary_semaphore command_smph; // Signaling the receipt of a command
};

class Command
{
  public:
    virtual ~Command() = default;
    virtual void execute(Task &task) = 0;
};

class TaskPool
{
  public:
    TaskPool() = default;
    ~TaskPool() = default;
    void add_task(std::unique_ptr<Task> &&task);
    void add_command_to(std::string name, std::shared_ptr<Command> command);
    void start(const std::string name);
    void stop(const std::string name);
    void start_all();
    void stop_all();

  private:
    std::shared_mutex tasks_lock;
    std::map<std::string, std::unique_ptr<Task>> tasks;
};

} // namespace ybotln

#endif
