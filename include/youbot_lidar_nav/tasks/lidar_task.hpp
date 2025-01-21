#ifndef LIDAR_TASK_HPP
#define LIDAR_TASK_HPP

#include "Urg_driver.h"
#include "youbot_lidar_nav/utils/task_pool.hpp"

#include <string>
#include <vector>

namespace ybotln {

class LidarTask : public Task
{
public:
    LidarTask(std::string name);
    ~LidarTask() = default;
    void update_parameters();
    bool connect_lidar();

    /**
     * @brief Get the distance object
     *
     * @param data Distance data array [mm]
     * @param time_stamp Timestamp [msec]
     */
    bool get_distance(std::vector<long>& data, std::vector<double> radian, long& time_stamp);

private:
    void task() override;

    qrk::Urg_driver urg;
    std::string device_name;
    int baudrate;
    double viewing_angle;
    bool lidar_alive = false;
};

class GetDistanceRequest : public Request
{
public:
    GetDistanceRequest() = default;
    void execute(Task& task) override;
    void data(std::vector<long>& length, std::vector<double> radian, long& time_stamp);

private:
    std::vector<long> length_;
    std::vector<double> radian_;
    long time_stamp_;
};

}  // namespace ybotln

#endif
