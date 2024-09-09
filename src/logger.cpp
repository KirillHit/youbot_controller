#include <iostream>

#include "youbot_lidar_nav/logger.hpp"

namespace youbot_nav
{

Logger &Logger::get_logger()
{
    static Logger single_instance;
    return single_instance;
}


Logger::Logger()
{
    
}

} // namespace youbot_nav