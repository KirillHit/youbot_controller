#include "youbot_lidar_nav/driver.hpp"
#include <algorithm>

namespace ybotln
{

Driver::Driver() : youbot_base("youbot-base", std::string(SOURCE_DIR) + "config/") {}

Driver::~Driver()
{
    stop();
}

void Driver::stop()
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

DriverTask::DriverTask(std::string name) : Task(name) {}

void DriverTask::task()
{
    while (!stop_flag)
    {
        try
        {
            LOGGER_STREAM(MSG_LVL::INFO, "Connecting to the youbot base...");
            driver = std::make_unique<Driver>();
        }
        catch (const std::runtime_error &e)
        {
            LOGGER_STREAM(MSG_LVL::ERROR, e.what());
        }
        std::this_thread::sleep_for(
            std::chrono::milliseconds(PARAMETERS.get<int>("driver/reconnect_delay")));
    }
}

} // namespace ybotln
