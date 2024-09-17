#ifndef THREAD_POOL_HPP
#define THREAD_POOL_HPP

#include <atomic>
#include <map>
#include <string>
#include <thread>

namespace ybotln
{

class TaskThread
{
  public:
    TaskThread(std::string name = "");
    ~TaskThread();
    void start();
    void stop();
    void wait();
    virtual void task() = 0;

  protected:
    std::atomic<bool> stop_flag;

  private:
    void task_dec();
    std::string get_name() const;
    std::string task_name;
    std::thread task_thread;
    std::atomic<bool> running;
};

} // namespace ybotln

#endif
