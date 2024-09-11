#include <iostream>
#include <vector>

#include "yaml-cpp/yaml.h"
#include "youbot_driver/youbot/YouBotBase.hpp"
#include "youbot_lidar_nav/logger.hpp"

using namespace ybot_ln;

void init_config(const std::string config_path)
{
    LOGGER_STREAM(MSG_LVL::INFO, "Loading parameters...");

    YAML::Node config =
        YAML::LoadFile(std::string(SOURCE_DIR) + "config/youbot_config.yaml");

    Logger &logger = Logger::get_logger();
    logger.set_debug(config["logger"]["debug"].as<bool>());
    logger.set_save(config["logger"]["save"].as<bool>(),
                    config["logger"]["save_path"].as<std::string>());
}

std::string parse(int argc, char *argv[])
{
    if (argc == 0)
    {
        return "";
    }

    if (argc > 1)
    {
        LOGGER_STREAM(MSG_LVL::ERROR, "Too many input parameters! Use: "
                                      "YoubotLidarNav <config file path>");
        return "";
    }

    return argv[0];
}

int main(int argc, char *argv[])
{
    LOGGER_STREAM(MSG_LVL::INFO, "Start initialization...");

    try
    {
        init_config(parse(argc, argv));
    }
    catch (const YAML::BadFile &e)
    {
        LOGGER_STREAM(MSG_LVL::ERROR, "Failed to load parameters. " << e.msg);
    }
    catch (const YAML::RepresentationException &e)
    {
        LOGGER_STREAM(MSG_LVL::ERROR, "Failed to load parameters. " << e.msg);
    }
    
    return 0;
}