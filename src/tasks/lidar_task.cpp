#include "youbot_lidar_nav/tasks/lidar_task.hpp"

#include "youbot_lidar_nav/utils/logger.hpp"
#include "youbot_lidar_nav/utils/parameter_server.hpp"

namespace ybotln {

LidarTask::LidarTask(std::string name) : Task(name)
{
    update_parameters();
}

void LidarTask::update_parameters()
{
    device_name = PARAMETERS.get<std::string>("lidar/device_name");
    baudrate = PARAMETERS.get<int>("lidar/baudrate");
    viewing_angle = PARAMETERS.get<double>("lidar/viewing_angle");
}

bool LidarTask::connect_lidar()
{
    if (!urg.open(device_name.c_str(), static_cast<long>(baudrate), qrk::Urg_driver::Serial))
    {
        LOGGER_STREAM(
            MSG_LVL::ERROR,
            "Failed to connect to lidar via " << device_name << "! Error text: " << urg.what());
        return false;
    }
    if (!urg.set_scanning_parameter(urg.deg2step(-viewing_angle), urg.deg2step(viewing_angle)))
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
        if (!lidar_alive)
        {
            lidar_alive = connect_lidar();
        }
        wait_process_commands();
    }
}

/**
 * @brief Returns lidar measurements
 * 
 * @param length Distance data array [mm]
 * @param radian Angle (in radians) for each step. The front of the sensor corresponds to 0. 
 * Uses [urg_index2rad](https://urgnetwork.sourceforge.net/html/library_tutorial_page.html#library_tutorial_xy)
 * @param time_stamp Timestamp [msec]
 */
bool LidarTask::get_distance(std::vector<long>& length, std::vector<double> radian, long& time_stamp)
{
    if (!lidar_alive)
    {
        LOGGER_STREAM(MSG_LVL::WARN, "Lidar is dead 0_0");
        return false;
    }
    bool res =
        urg.start_measurement(qrk::Urg_driver::Distance, 1) & urg.get_distance(length, &time_stamp);
    if (!res)
    {
        LOGGER_STREAM(MSG_LVL::ERROR, "Failed to get data from lidar! Error text: " << urg.what());
        urg.close();
        lidar_alive = false;
        return false;
    }
    size_t count = length.size();
    radian.clear();
    radian.reserve(count);
    for (int idx = 0; idx < count; ++idx)
    {
        radian.push_back(urg.index2rad(idx));
    }
    return true;
}

/**************************** Command interface ****************************/

void GetDistanceRequest::execute(Task& task)
{
    std::lock_guard<std::mutex> lock(request_lock);
    LidarTask& lidar_task = dynamic_cast<LidarTask&>(task);
    result_ = lidar_task.get_distance(length_, radian_, time_stamp_);
}

void GetDistanceRequest::data(std::vector<long>& length, std::vector<double> radian, long& time_stamp)
{
    std::lock_guard<std::mutex> lock(request_lock);
    length.swap(length_);
    radian.swap(radian_);
    time_stamp = time_stamp_;
}

}  // namespace ybotln
