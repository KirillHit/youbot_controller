#include <chrono>
#include <iostream>
#include <csignal>
#include <thread>
#include <vector>

#include "yaml-cpp/yaml.h"
#include "youbot_lidar_nav/driver.hpp"
#include "youbot_lidar_nav/logger.hpp"
#include "youbot_lidar_nav/parameter_server.hpp"

using namespace ybotln;

void sigint_handler(int signal)
{
    LOGGER_STREAM(MSG_LVL::INFO, "Caught terminal interrupt signal. Stopping...");
    PARAMETERS.set("running", false);
}

void init_default_parameters()
{
    PARAMETERS.set("running", true);
    PARAMETERS.set<double>("max_leaner_vel", 0.5);
    PARAMETERS.set<int>("reconnect_delay", 1);
}

void init_config()
{
    std::string config_path =
        std::string(SOURCE_DIR) + "config/youbot_config.yaml";

    LOGGER_STREAM(MSG_LVL::INFO,
                  "Loading parameters from file \"" << config_path << "\"...");

    YAML::Node config = YAML::LoadFile(config_path);

    PARAMETERS.set("log_debug", config["logger"]["debug"].as<bool>());
    PARAMETERS.set("log_save_path",
                   config["logger"]["save_path"].as<std::string>());
    PARAMETERS.set("log_save", config["logger"]["save"].as<bool>());
    PARAMETERS.set("max_leaner_vel",
                   config["driver"]["max_leaner_vel"].as<double>());
    PARAMETERS.set("reconnect_delay",
                   config["driver"]["reconnect_delay"].as<int>());

    LOGGER_STREAM(MSG_LVL::INFO, "Loading parameters completed successfully.");
}

int main()
{
    LOGGER_STREAM(MSG_LVL::INFO, "Start initialization...");

    signal(SIGINT, sigint_handler);

    init_default_parameters();

    try
    {
        init_config();
    }
    catch (const YAML::BadFile &e)
    {
        LOGGER_STREAM(MSG_LVL::ERROR, "Failed to load parameters! " << e.msg);
    }
    catch (const YAML::RepresentationException &e)
    {
        LOGGER_STREAM(MSG_LVL::ERROR, "Failed to load parameters! " << e.msg);
    }

    while (PARAMETERS.get<bool>("running"))
    {
        try
        {
            // def control();
            
            Driver youbot_driver;
        }
        catch (const std::runtime_error &e)
        {
            LOGGER_STREAM(MSG_LVL::ERROR, e.what());
        }

        std::this_thread::sleep_for(
            std::chrono::milliseconds(PARAMETERS.get<int>("reconnect_delay")));
    }

    return 0;
}