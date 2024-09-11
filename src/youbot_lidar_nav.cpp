#include <iostream>
#include <vector>

#include "yaml-cpp/yaml.h"
#include "youbot_driver/youbot/YouBotBase.hpp"
#include "youbot_lidar_nav/logger.hpp"
#include "youbot_lidar_nav/parameter_server.hpp"

using namespace ybotln;

void init_config(std::string config_path)
{
    LOGGER_STREAM(MSG_LVL::INFO, "Loading parameters...");

    if (config_path.empty()) {
        config_path = std::string(SOURCE_DIR) + "config/youbot_config.yaml";
    }
    
    YAML::Node config =
        YAML::LoadFile(config_path);

    Logger &logger = Logger::get_logger();
    logger.set_debug(config["logger"]["debug"].as<bool>());
    logger.set_save(config["logger"]["save"].as<bool>(),
                    config["logger"]["save_path"].as<std::string>());

    LOGGER_STREAM(MSG_LVL::INFO, "loading parameters completed successfully.");
}

std::string parse(int argc, char *argv[])
{
    if (argc == 1)
    {
        return "";
    }

    if (argc > 2)
    {
        LOGGER_STREAM(MSG_LVL::ERROR, "Too many input parameters! Use: "
                                      "YoubotLidarNav <config file path>");
        return "";
    }

    return argv[1];
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

    ParameterServer::get_parameters().set<bool>("lol", true);

    return 0;
}