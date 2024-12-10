#include "youbot_lidar_nav/tasks/planner_task.hpp"

#include "youbot_lidar_nav/tasks/driver_task.hpp"
#include "youbot_lidar_nav/tasks/lidar_task.hpp"
#include "youbot_lidar_nav/utils/logger.hpp"
#include "youbot_lidar_nav/utils/parameter_server.hpp"

#include <chrono>
#include <memory>

namespace ybotln {

PlannerTask::PlannerTask(std::string name) : Task(name)
{
    update_parameters();
}

void PlannerTask::update_parameters() {}

void PlannerTask::task()
{
    while (!stop_flag)
    {
        auto distant_request = std::make_shared<GetDistanceRequest>();
        emit_command("lidar", distant_request);

        auto odom_request = std::make_shared<GetOdomRequest>();
        emit_command("driver", odom_request);

        distant_request->wait_command();
        odom_request->wait_command();

        if (distant_request->result())
        {
            std::vector<long> data;
            long time_stamp;
            distant_request->data(data, time_stamp);
            LOGGER_STREAM(MSG_LVL::ERROR, "Failed to get data from lidar!");
            LOGGER_STREAM(MSG_LVL::DEBUG, time_stamp << " : " << data.size() << " : ");
        }

        if (odom_request->result())
        {
            double longitudinal, transversal, angular;
            odom_request->data(longitudinal, transversal, angular);
            LOGGER_STREAM(MSG_LVL::ERROR, "Failed to get data from driver!");
        }

        try_process_commands_for(std::chrono::milliseconds(1000));
    }
}

}  // namespace ybotln
