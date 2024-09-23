#ifndef DRIVER_HPP
#define DRIVER_HPP

#include <vector>

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
    void set_route(std::queue<RouteStep>&& n_route);

  private:
    void task() override;

    std::unique_ptr<Driver> driver;
    std::queue<RouteStep> route;
    int reconnect_delay;
};

class RouteCommand : public Command
{
  public:
    RouteCommand() = default;
    void execute(Task &task) override;
    void add_step(const RouteStep &step);

  private:
    std::queue<RouteStep> new_route;
};

} // namespace ybotln

#endif // DRIVER_HPP