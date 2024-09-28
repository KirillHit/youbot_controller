#ifndef PLANNER_H
#define PLANNER_H

struct RouteStep
{
    double longitudinal_vel = 0; // m/s
    double transversal_vel = 0;  // m/s
    double angular_vel = 0;      // rad/s
    int duration;                // ms
};

#endif // PLANNER_H
