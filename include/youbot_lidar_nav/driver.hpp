#ifndef DRIVER_HPP
#define DRIVER_HPP

#include "logger.hpp"
#include "parameter_server.hpp"
#include "youbot_driver/youbot/YouBotBase.hpp"

namespace ybotln
{

class Driver : public IParametersObserver
{
  public:
    Driver();
    ~Driver();
    void init_parameters();
    void set_speed(const double &longitudinal_vel,
                   const double &transversal_vel, const double &angular_vel);
    void stop();
    void handle_event(const std::string name) override;

  private:
    youbot::YouBotBase myYouBotBase;

    double max_leaner_vel = 0.5;
};

} // namespace ybotln

#endif // DRIVER_HPP