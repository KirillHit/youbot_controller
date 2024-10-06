#include "youbot_lidar_nav/tasks/planner_task.hpp"
#include "youbot_lidar_nav/utils/logger.hpp"
#include "youbot_lidar_nav/utils/parameter_server.hpp"
#include "youbot_lidar_nav/tasks/driver_task.hpp"
#include "youbot_lidar_nav/tasks/tcp_server_task.hpp"

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
        wait_process_commands();
    }
}


} // namespace ybotln
