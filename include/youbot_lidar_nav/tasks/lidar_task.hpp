#ifndef LIDAR_TASK_HPP
#define LIDAR_TASK_HPP

#include <string>
#include <vector>

#include "Urg_driver.h"
#include "youbot_lidar_nav/utils/task_pool.hpp"

namespace ybotln
{

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
    bool get_distance(std::vector<long> &data, long &time_stamp);

  private:
    void task() override;

    qrk::Urg_driver urg;
    std::string device_name;
    int baudrate;
    int reconnect_delay = 5000; // ms
    bool lidar_alive = false;
};

class GetDistanceRequest : public Request
{
  public:
    GetDistanceRequest() = default;
    void request(Task &task) override;
    bool get_result(std::vector<long> &data, long &time_stamp);

  private:
    std::vector<long> data_;
    long time_stamp_;
    bool result_;
};

} // namespace ybotln

#endif
