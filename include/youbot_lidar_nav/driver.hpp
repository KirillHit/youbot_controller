#ifndef DRIVER_HPP
#define DRIVER_HPP

#include "utils/logger.hpp"
#include "utils/parameter_server.hpp"
#include "utils/task_thread.hpp"
#include "youbot_driver/youbot/YouBotBase.hpp"

namespace ybotln
{

class Driver
{
  public:
    Driver();
    ~Driver();
    void set_speed(const double &longitudinal_vel,
                   const double &transversal_vel, const double &angular_vel);
    void stop();
    void update_parameters();

  private:
    youbot::YouBotBase youbot_base;
    double max_leaner_vel = 0.5;
};

class DriverTask : public Task
{
  public:
    DriverTask(std::string name);
    ~DriverTask() = default;

  private:
    void task() override;

    std::unique_ptr<Driver> driver;
};

} // namespace ybotln

#endif // DRIVER_HPP