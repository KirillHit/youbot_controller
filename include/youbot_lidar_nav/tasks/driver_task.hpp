#ifndef DRIVER_TASK_HPP
#define DRIVER_TASK_HPP

#include <vector>

#include "youbot_driver/youbot/YouBotBase.hpp"
#include "youbot_lidar_nav/utils/logger.hpp"
#include "youbot_lidar_nav/utils/parameter_server.hpp"
#include "youbot_lidar_nav/utils/task_pool.hpp"

namespace ybotln
{

class Driver
{
  public:
    Driver();
    ~Driver();
    void update_parameters();
    void set_speed(const double &longitudinal_vel, const double &transversal_vel,
                   const double &angular_vel);
    void set_stop();

  private:
    youbot::YouBotBase youbot_base;
    double max_leaner_vel = 0.5;
};

struct RouteStep
{
    double longitudinal_vel; // m/s
    double transversal_vel;  // m/s
    double angular_vel;      // rad/s
    int duration;            // ms
};

class DriverTask : public Task
{
  public:
    DriverTask(std::string name);
    ~DriverTask() = default;
    void update_parameters();
    void add_route_steps(std::queue<RouteStep> &&n_route, const bool reset = false);

  private:
    void task() override;
    void spin_route();

    std::unique_ptr<Driver> driver;
    std::queue<RouteStep> route;
    int reconnect_delay = 5000; // ms
    bool break_route_step = false;
};

class RouteCommand : public Command
{
  public:
    RouteCommand() = default;
    void execute(Task &task) override;
    void add_step(const RouteStep &step);
    void set_reset(const bool reset_flag);

  private:
    std::queue<RouteStep> new_route;
    bool reset_route = true;
};

} // namespace ybotln

#endif // DRIVER_HPP