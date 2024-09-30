#ifndef LIDAR_HPP
#define LIDAR_HPP

#include "utils/logger.hpp"
#include "utils/parameter_server.hpp"
#include "utils/task_pool.hpp"
#include "Urg_driver.h"

namespace ybotln
{

class LidarTask : public Task
{
  public:
    LidarTask(std::string name);
    ~LidarTask() = default;
    void update_parameters();
    void init_lidar();

  private:
    void task() override;

    qrk::Urg_driver urg;
    std::string device_name;
    long baudrate_number;
    int reconnect_delay = 5000; // ms
};

} // namespace ybotln

#endif
