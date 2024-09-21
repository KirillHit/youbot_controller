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

  private:
    youbot::YouBotBase myYouBotBase;
};

class DriverTask : public Task
{
  public:
    DriverTask(std::string name);
    ~DriverTask() = default;

  private:
    void task() override;
    void init_parameters();

    double max_leaner_vel = 0.5;
};

} // namespace ybotln

#endif // DRIVER_HPP