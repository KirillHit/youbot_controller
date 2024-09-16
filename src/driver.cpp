#include "youbot_lidar_nav/driver.hpp"
#include <algorithm>

namespace ybotln
{

Driver::Driver()
    : myYouBotBase("youbot-base", std::string(SOURCE_DIR) + "config/")
{
    init_parameters();
    myYouBotBase.doJointCommutation();
    stop();
}

Driver::~Driver()
{
    PARAMETERS.remove_observer(this);
}

void Driver::init_parameters()
{
    PARAMETERS.add_observer("max_leaner_vel",
                            dynamic_cast<IParametersObserver *>(this));
    PARAMETERS.init_observer(dynamic_cast<IParametersObserver *>(this));
}

void Driver::handle_event(const std::string name)
{
    if (name == "max_leaner_vel")
    {
        double n_max_leaner_vel = PARAMETERS.get<double>("max_leaner_vel");
        if (n_max_leaner_vel < 0)
        {
            LOGGER_STREAM(MSG_LVL::ERROR, "The maximum linear speed must be "
                                          "greater than 0! Received value: "
                                              << n_max_leaner_vel);
            return;
        }
        max_leaner_vel = n_max_leaner_vel;
        return;
    }
}

void Driver::stop()
{
    youbot::JointVelocitySetpoint setVel;
    setVel.angularVelocity = 0 * radian_per_second;
    myYouBotBase.getBaseJoint(1).setData(setVel);
    myYouBotBase.getBaseJoint(2).setData(setVel);
    myYouBotBase.getBaseJoint(3).setData(setVel);
    myYouBotBase.getBaseJoint(4).setData(setVel);
}

void Driver::set_speed(const double &longitudinal_vel,
                       const double &transversal_vel, const double &angular_vel)
{

    quantity<si::velocity> longitudinalVelocity =
        std::clamp(longitudinal_vel, -max_leaner_vel, max_leaner_vel) *
        meter_per_second;
    quantity<si::velocity> transversalVelocity =
        std::clamp(transversal_vel, -max_leaner_vel, max_leaner_vel) *
        meter_per_second;
    quantity<si::angular_velocity> angularVelocity =
        angular_vel * radian_per_second;

    myYouBotBase.setBaseVelocity(longitudinalVelocity, transversalVelocity,
                                 angularVelocity);
}

} // namespace ybotln
