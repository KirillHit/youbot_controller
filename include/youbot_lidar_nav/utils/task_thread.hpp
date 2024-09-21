#ifndef THREAD_POOL_HPP
#define THREAD_POOL_HPP

#include <atomic>
#include <map>
#include <memory>
#include <mutex>
#include <queue>
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
    ~Task();
    void start();
    void stop();
    void join();
    std::string get_name() const;
    void set_task_poll(TaskPool * n_pool);
    void emit_command(std::string name, std::shared_ptr<Command> command);
    void process_commands();

  protected:
    std::atomic<bool> stop_flag;
    virtual void task() = 0;

  private:
    void task_dec();
    std::string log_name() const;
    std::string task_name;
    std::thread task_thread;
    std::atomic<bool> running;
    TaskPool *task_pool = nullptr;
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

    void add_task(std::unique_ptr<Task> task);
    void add_command(std::string name, std::shared_ptr<Command> command);
    std::queue<std::shared_ptr<Command>> get_commands(std::string name);

    void start(std::string name = "");
    void stop(std::string name = "");
    void start_all();
    void stop_all();

  private:
    std::mutex tasks_lock;
    std::map<std::string, std::pair<std::unique_ptr<Task>, std::queue<std::shared_ptr<Command>>>>
        tasks;
};

} // namespace ybotln

#endif
