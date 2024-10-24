#include "youbot_lidar_nav/tasks/lidar_task.hpp"
#include "youbot_lidar_nav/utils/logger.hpp"
#include "youbot_lidar_nav/utils/parameter_server.hpp"

namespace ybotln
{

LidarTask::LidarTask(std::string name) : Task(name)
{
    update_parameters();
}

void LidarTask::update_parameters()
{
    device_name = PARAMETERS.get<std::string>("lidar/device_name");
    baudrate = PARAMETERS.get<int>("lidar/baudrate");
}

bool LidarTask::connect_lidar()
{
    if (!urg.open(device_name.c_str(), static_cast<long>(baudrate), qrk::Urg_driver::Serial))
    {
        LOGGER_STREAM(MSG_LVL::ERROR, "Failed to connect to lidar via "
                                          << device_name << "! Error text: " << urg.what());
        return false;
    }
    if (!urg.set_scanning_parameter(urg.deg2step(-90), urg.deg2step(+90), 0))
    {
        LOGGER_STREAM(MSG_LVL::ERROR, "Failed to set lidar parameters! Error text: " << urg.what());
        return false;
    }
    if (!urg.start_measurement(qrk::Urg_driver::Distance, qrk::Urg_driver::Infinity_times, 0))
    {
        LOGGER_STREAM(MSG_LVL::ERROR, "Failed to set lidar parameters! Error text: " << urg.what());
        return false;
    }
    LOGGER_STREAM(MSG_LVL::INFO, "Lidar successfully connected to the device " << device_name);
    return true;
}

void LidarTask::task()
{
    while (!stop_flag)
    {
        if (lidar_alive)
        {
            wait_process_commands();
            continue;
        }
        lidar_alive = connect_lidar();
        try_process_commands_for(std::chrono::milliseconds(reconnect_delay));
    }
}

bool LidarTask::get_distance(std::vector<long> &data, long &time_stamp)
{
    if (!lidar_alive)
    {
        LOGGER_STREAM(MSG_LVL::DEBUG, "Lidar is dead 0_0");
        return false;
    }
    if (!urg.get_distance(data, &time_stamp))
    {
        LOGGER_STREAM(MSG_LVL::ERROR, "Failed to get data from lidar! Error text: " << urg.what());
        return false;
    }
    return true;
}

void GetDistanceRequest::execute(Task &task)
{
    std::lock_guard<std::mutex> lock(request_lock);
    LidarTask &lidar_task = dynamic_cast<LidarTask &>(task);
    result_ = lidar_task.get_distance(data_, time_stamp_);
}

void GetDistanceRequest::data(std::vector<long> &data, long &time_stamp)
{
    std::lock_guard<std::mutex> lock(request_lock);
    data.swap(data_);
    time_stamp = time_stamp_;
}

} // namespace ybotln
