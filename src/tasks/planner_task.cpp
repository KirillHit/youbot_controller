#include <chrono>
#include <memory>

#include "youbot_lidar_nav/tasks/driver_task.hpp"
#include "youbot_lidar_nav/tasks/lidar_task.hpp"
#include "youbot_lidar_nav/tasks/planner_task.hpp"
#include "youbot_lidar_nav/utils/logger.hpp"
#include "youbot_lidar_nav/utils/parameter_server.hpp"

namespace ybotln
{

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
        distant_request->try_process_request_for(std::chrono::milliseconds(1000));
        std::vector<long> data;
        long time_stamp;
        if (!distant_request->get_result(data, time_stamp))
        {
            LOGGER_STREAM(MSG_LVL::ERROR, "Failed to get data from lidar!");
        }
        LOGGER_STREAM(MSG_LVL::DEBUG, time_stamp << " : " << data.size()<< " : ");
        try_process_commands_for(std::chrono::milliseconds(1000));
    }
}

} // namespace ybotln
