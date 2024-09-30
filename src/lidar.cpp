#include "youbot_lidar_nav/lidar.hpp"

namespace ybotln
{

LidarTask::LidarTask(std::string name) : Task(name)
{
    update_parameters();
}

void LidarTask::update_parameters()
{
    std::string device_name = PARAMETERS.get<std::string>("lidar/device_name");
    long baudrate = PARAMETERS.get<long>("lidar/baudrate");
}

void LidarTask::init_lidar()
{
    while (!urg.open(device_name.c_str(), baudrate_number, qrk::Urg_driver::Serial))
    {
        LOGGER_STREAM(MSG_LVL::ERROR,
                      "Failed to connect to lidar via "
                          << device_name
                          << "! Make sure the program has permission to access the device.");
        try_process_commands_for(std::chrono::milliseconds(reconnect_delay));
    }
}

void LidarTask::task()
{
    while (!stop_flag)
    {
        wait_process_commands();
    }
}

} // namespace ybotln
