#include <algorithm>

#include "youbot_lidar_nav/tasks/driver_task.hpp"
#include "youbot_lidar_nav/utils/logger.hpp"
#include "youbot_lidar_nav/utils/parameter_server.hpp"

namespace ybotln
{

Driver::Driver() : youbot_base("youbot-base", std::string(SOURCE_DIR) + "config/")
{
    update_parameters();
    youbot_base.doJointCommutation();
}

Driver::~Driver()
{
    set_stop();
}

void Driver::update_parameters()
{
    max_leaner_vel = PARAMETERS.get<double>("driver/max_leaner_vel");
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

void Driver::get_odom(double &longitudinal, double &transversal, double &angular)
{
    quantity<si::length> longitudinal_si, transversal_si;
    quantity<si::plane_angle> angular_si;
    youbot_base.getBasePosition(longitudinal_si, transversal_si, angular_si);
    longitudinal = quantity_cast<double>(longitudinal_si);
    transversal = quantity_cast<double>(longitudinal_si);
    angular = quantity_cast<double>(longitudinal_si);
}

void Driver::set_odom(const double &longitudinal, const double &transversal, const double &angular)
{
    quantity<si::length> longitudinal_si = longitudinal * meter;
    quantity<si::length> transversal_si = transversal * meter;
    quantity<si::plane_angle> angular_si = angular * radian;
    youbot_base.setBasePosition(longitudinal_si, transversal_si, angular_si);
}

DriverTask::DriverTask(std::string name) : Task(name)
{
    update_parameters();
}

void DriverTask::update_parameters() {}

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
        try_process_commands_for(std::chrono::milliseconds(reconnect_delay));
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

            auto end_time =
                std::chrono::steady_clock::now() + std::chrono::milliseconds(task.duration);
            while (try_process_commands_until(end_time))
            {
                if (break_route_step)
                {
                    break_route_step = false;
                    break;
                }
            }
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
        break_route_step = true;
        return;
    }
    for (; !n_route.empty(); n_route.pop())
    {
        route.push(n_route.front());
    }
}

void DriverTask::get_odom()
{
    double longitudinal, transversal, angular;
    driver->get_odom(longitudinal, transversal, angular);
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

void GetOdomRequest::request(Task &task)
{
    std::lock_guard<std::mutex> lock(request_lock);
    DriverTask &driver_task = dynamic_cast<DriverTask &>(task);
    // result_ = lidar_task.get_distance(data_, time_stamp_);
}

bool GetOdomRequest::get_result(std::vector<long> &data, long &time_stamp)
{
    std::lock_guard<std::mutex> lock(request_lock);
    data.swap(data_);
    time_stamp = time_stamp_;
    return result_;
}

} // namespace ybotln
