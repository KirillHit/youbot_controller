#include "youbot_driver/youbot/YouBotBase.hpp"

using namespace youbot;

int main()
{
    try
    {
        // creates a YouBotBase instance with the name "youbot-base"
        // which is also the name of the config file
        YouBotBase myYouBotBase("youbot-base");
        // do the sine commutation of the base joints
        myYouBotBase.doJointCommutation();
        quantity<si::velocity> longitudinalVelocity = 0.2 * meter_per_second;
        quantity<si::velocity> transversalVelocity = 0.0 * meter_per_second;
        quantity<si::angular_velocity> angularVelocity = 0 * radian_per_second;
        quantity<si::velocity> actualLongitudinalVelocity =
            0 * meter_per_second;
        quantity<si::velocity> actualTransversalVelocity = 0 * meter_per_second;
        quantity<si::angular_velocity> actualAngularVelocity =
            0 * radian_per_second;
        quantity<si::length> actualLongitudinalPose = 0 * meter;
        quantity<si::length> actualTransversalPose = 0 * meter;
        quantity<si::plane_angle> actualAngle = 0 * radian;
        // sets the base cartesian velocity
        myYouBotBase.setBaseVelocity(longitudinalVelocity, transversalVelocity,
                                     angularVelocity);
        // reads the base cartesian velocity
        myYouBotBase.getBaseVelocity(actualLongitudinalVelocity,
                                     actualTransversalVelocity,
                                     actualAngularVelocity);
        // reads the base cartesian position which have been calculated from the
        // odometry
        myYouBotBase.getBasePosition(actualLongitudinalPose,
                                     actualTransversalPose, actualAngle);
        // print the actual cartesian velocity
        LOG(info) << "actual velocity longitudinal: "
                  << actualLongitudinalVelocity
                  << " transversal: " << actualTransversalVelocity
                  << " angular: " << actualAngularVelocity;
        // command base joint 1 a velocity of 2 radian per second
        JointVelocitySetpoint setVel;
        setVel.angularVelocity = 2 * radian_per_second;
        myYouBotBase.getBaseJoint(1).setData(setVel);
    }
    catch (std::exception &e)
    {
        std::cout << e.what() << std::endl;
    }
    catch (...)
    {
        std::cout << "unhandled exception" << std::endl;
    }

    return 0;
}