#ifndef PLANNER_TASK_HPP
#define PLANNER_TASK_HPP

#include "youbot_lidar_nav/utils/task_pool.hpp"

namespace ybotln {

class PlannerTask : public Task
{
public:
    PlannerTask(std::string name);
    ~PlannerTask() = default;
    void update_parameters();

private:
    void task() override;
    void spin_route();
};

}  // namespace ybotln

#endif