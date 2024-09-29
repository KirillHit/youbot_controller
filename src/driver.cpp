#include "youbot_lidar_nav/driver.hpp"
#include <algorithm>

namespace ybotln
{

Driver::Driver() : youbot_base("youbot-base", std::string(SOURCE_DIR) + "config/")
{
    update_parameters();
    youbot_base.doJointCommutation();
}

void Driver::update_parameters()
{
    max_leaner_vel = PARAMETERS.get<double>("driver/max_leaner_vel");
}

Driver::~Driver()
{
    set_stop();
}

void Driver::set_stop()
{
    youbot::JointVelocitySetpoint setVel;
    setVel.angularVelocity = 0 * radian_per_second;
    youbot_base.getBaseJoint(1).setData(setVel);
    youbot_base.getBaseJoint(2).setData(setVel);
    youbot_base.getBaseJoint(3).setData(setVel);
    youbot_base.getBaseJoint(4).setData(setVel);
}

void Driver::set_speed(const double &longitudinal_vel, const double &transversal_vel,
                       const double &angular_vel)
{

    quantity<si::velocity> longitudinalVelocity =
        std::clamp(longitudinal_vel, -max_leaner_vel, max_leaner_vel) * meter_per_second;
    quantity<si::velocity> transversalVelocity =
        std::clamp(transversal_vel, -max_leaner_vel, max_leaner_vel) * meter_per_second;
    quantity<si::angular_velocity> angularVelocity = angular_vel * radian_per_second;

    youbot_base.setBaseVelocity(longitudinalVelocity, transversalVelocity, angularVelocity);
}

DriverTask::DriverTask(std::string name) : Task(name)
{
    update_parameters();
}

void DriverTask::update_parameters()
{
    reconnect_delay = PARAMETERS.get<int>("driver/reconnect_delay");
}

void DriverTask::task()
{
    while (!stop_flag)
    {
        try
        {
            LOGGER_STREAM(MSG_LVL::INFO, "Connecting to the youbot base...");
            driver = std::make_unique<Driver>();
            spin_route();
        }
        catch (const std::runtime_error &e)
        {
            LOGGER_STREAM(MSG_LVL::ERROR, e.what());
        }
        try_process_commands(std::chrono::milliseconds(reconnect_delay));
    }
}

void DriverTask::spin_route()
{
    while (!stop_flag)
    {
        if (!route.empty())
        {
            ybotln::RouteStep task = route.front();
            route.pop();
            driver->set_speed(task.longitudinal_vel, task.transversal_vel, task.angular_vel);
            try_process_commands(std::chrono::milliseconds(task.duration));
            // TODO
        }
        else
        {
            driver->set_stop();
            wait_process_commands();
        }
    }
}

void DriverTask::add_route_steps(std::queue<RouteStep> &&n_route, const bool reset)
{
    if (reset)
    {
        route = std::move(n_route);
        return;
    }
    for (; !n_route.empty(); n_route.pop())
    {
        route.push(n_route.front());
    }
}

void RouteCommand::execute(Task &task)
{
    DriverTask &driver_task = dynamic_cast<DriverTask &>(task);
    driver_task.add_route_steps(std::move(new_route), reset_route);
}

void RouteCommand::add_step(const RouteStep &step)
{
    new_route.push(step);
}

void RouteCommand::set_reset(const bool reset_flag)
{
    reset_route = reset_flag;
}

} // namespace ybotln
