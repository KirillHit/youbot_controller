#include <iostream>
#include <vector>

#include "yaml-cpp/yaml.h"
#include "youbot_driver/youbot/YouBotBase.hpp"
#include "youbot_lidar_nav/logger.hpp"
#include "youbot_lidar_nav/parameter_server.hpp"

using namespace ybotln;

void init_config()
{
    LOGGER_STREAM(MSG_LVL::INFO, "Loading parameters...");

    std::string config_path =
        std::string(SOURCE_DIR) + "config/youbot_config.yaml";

    if (PARAMETERS.exist<std::string>("config_path"))
    {
        config_path = PARAMETERS.get<std::string>("config_path");
    }

    YAML::Node config = YAML::LoadFile(config_path);

    PARAMETERS.set("debug", config["logger"]["debug"].as<bool>());
    PARAMETERS.set("save", config["logger"]["save"].as<bool>());
    PARAMETERS.set("save_path", config["logger"]["save_path"].as<std::string>());

    LOGGER_STREAM(MSG_LVL::INFO, "Loading parameters completed successfully.");
}

void parse(int argc, char *argv[])
{
    if (argc == 1)
    {
        return;
    }

    if (argc > 2)
    {
        LOGGER_STREAM(MSG_LVL::ERROR, "Too many input parameters! Use: "
                                      "YoubotLidarNav <config file path>");
        return;
    }

    PARAMETERS.set<std::string>("config_path", argv[1]);
}

int main(int argc, char *argv[])
{
    LOGGER_STREAM(MSG_LVL::INFO, "Start initialization...");

    parse(argc, argv);

    try
    {
        init_config();
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