#include <iostream>
#include <vector>

#include "youbot_driver/youbot/YouBotBase.hpp"
#include "youbot_lidar_nav/logger.hpp"

using namespace std;
using namespace youbot;

int main()
{

    try
    {

        ConfigFile configfile("/applications.cfg", std::string(CONFIG_DIR));

        int ch = 0;
        double linearVel = 0.05; // meter_per_second
        double angularVel = 0.2; // radian_per_second

        configfile.readInto(linearVel, "KeyboardRemoteContol",
                            "TanslationalVelocity_[meter_per_second]");
        configfile.readInto(angularVel, "KeyboardRemoteContol",
                            "RotationalVelocity_[radian_per_second]");

        YouBotBase myYouBotBase("youbot-base", YOUBOT_CONFIGURATIONS_DIR);
        myYouBotBase.doJointCommutation();

        JointVelocitySetpoint setVel;
        quantity<si::velocity> longitudinalVelocity = 0 * meter_per_second;
        quantity<si::velocity> transversalVelocity = 0 * meter_per_second;
        quantity<si::angular_velocity> angularVelocity = 0 * radian_per_second;

        while (running)
        {

            switch (ch)
            {
            case KEY_DOWN:
                longitudinalVelocity = -linearVel * meter_per_second;
                transversalVelocity = 0 * meter_per_second;
                angularVelocity = 0 * radian_per_second;
                LOG(info) << "drive backward";
                printf("\r");
                break;
            case KEY_UP:
                longitudinalVelocity = linearVel * meter_per_second;
                transversalVelocity = 0 * meter_per_second;
                angularVelocity = 0 * radian_per_second;
                LOG(info) << "drive forward";
                printf("\r");
                break;
            case KEY_LEFT:
                transversalVelocity = linearVel * meter_per_second;
                longitudinalVelocity = 0 * meter_per_second;
                angularVelocity = 0 * radian_per_second;
                LOG(info) << "drive left";
                printf("\r");
                break;
            case KEY_RIGHT:
                transversalVelocity = -linearVel * meter_per_second;
                longitudinalVelocity = 0 * meter_per_second;
                angularVelocity = 0 * radian_per_second;
                LOG(info) << "drive right";
                printf("\r");
                break;
            case 'y':
                angularVelocity = angularVel * radian_per_second;
                transversalVelocity = 0 * meter_per_second;
                longitudinalVelocity = 0 * meter_per_second;
                LOG(info) << "turn right";
                printf("\r");
                break;
            case 'x':
                angularVelocity = -angularVel * radian_per_second;
                transversalVelocity = 0 * meter_per_second;
                longitudinalVelocity = 0 * meter_per_second;
                LOG(info) << "turn left";
                printf("\r");
                break;

            default:
                longitudinalVelocity = 0 * meter_per_second;
                transversalVelocity = 0 * meter_per_second;
                angularVelocity = 0 * radian_per_second;
                LOG(info) << "stop";
                printf("\r");
                break;
            }

            myYouBotBase.setBaseVelocity(longitudinalVelocity,
                                         transversalVelocity, angularVelocity);
        }

        setVel.angularVelocity = 0 * radian_per_second;
        myYouBotBase.getBaseJoint(1).setData(setVel);
        myYouBotBase.getBaseJoint(2).setData(setVel);
        myYouBotBase.getBaseJoint(3).setData(setVel);
        myYouBotBase.getBaseJoint(4).setData(setVel);
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