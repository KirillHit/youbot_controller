#include <chrono>
#include <csignal>
#include <iostream>
#include <semaphore>
#include <thread>
#include <vector>

#include "yaml-cpp/yaml.h"
#include "youbot_lidar_nav/driver.hpp"
#include "youbot_lidar_nav/tcp_server.hpp"
#include "youbot_lidar_nav/utils/logger.hpp"
#include "youbot_lidar_nav/utils/parameter_server.hpp"
#include "youbot_lidar_nav/utils/task_pool.hpp"

using namespace ybotln;

std::binary_semaphore signal_smph(0);
void sigint_handler(int signal)
{
    LOGGER_STREAM(MSG_LVL::INFO, "Caught terminal interrupt signal. Stopping...");
    signal_smph.release();
}

void init_default_parameters()
{
    PARAMETERS.set<double>("logger/debug", false);
    PARAMETERS.set<double>("logger/save_path", false);
    PARAMETERS.set<std::string>("logger/save", "");
    PARAMETERS.set<double>("driver/max_leaner_vel", 0.5);
    PARAMETERS.set<int>("tcp_server/port", 10000);
    PARAMETERS.set<int>("tcp_server/timeout", 500);
    PARAMETERS.set<std::string>("lidar/device_name", "/dev/ttyACM0");
    PARAMETERS.set<long>("lidar/baudrate", 115200);
}

void init_config()
{
    std::string config_path = std::string(SOURCE_DIR) + "config/youbot_config.yaml";

    LOGGER_STREAM(MSG_LVL::INFO, "Loading parameters from file \"" << config_path << "\"...");

    YAML::Node config = YAML::LoadFile(config_path);

    PARAMETERS.set("logger/debug", config["logger"]["debug"].as<bool>());
    PARAMETERS.set("logger/save_path", config["logger"]["save_path"].as<std::string>());
    PARAMETERS.set("logger/save", config["logger"]["save"].as<bool>());
    PARAMETERS.set("driver/max_leaner_vel", config["driver"]["max_leaner_vel"].as<double>());
    PARAMETERS.set("tcp_server/port", config["tcp_server"]["port"].as<int>());
    PARAMETERS.set("tcp_server/timeout", config["tcp_server"]["timeout"].as<int>());
    PARAMETERS.set("tcp_server/port", config["tcp_server"]["port"].as<int>());
    PARAMETERS.set("tcp_server/timeout", config["tcp_server"]["timeout"].as<int>());


    LOGGER_STREAM(MSG_LVL::INFO, "Loading parameters completed successfully");
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

    Logger::get_logger().update_parameters();

    TaskPool task_pool;

    auto driver_task = std::make_unique<DriverTask>("driver");
    task_pool.add_task(std::move(driver_task));

    auto tcp_server_task = std::make_unique<TcpServerTask>("tcp_server");
    task_pool.add_task(std::move(tcp_server_task));

    task_pool.start_all();

    signal_smph.acquire();

    task_pool.stop_all();

    LOGGER_STREAM(MSG_LVL::INFO, "All threads stopped. Exiting program.");

    return 0;
}