#include "yaml-cpp/yaml.h"
#include "youbot_lidar_nav/tasks/driver_task.hpp"
#include "youbot_lidar_nav/tasks/lidar_task.hpp"
#include "youbot_lidar_nav/tasks/planner_task.hpp"
#include "youbot_lidar_nav/tasks/tcp_server_task.hpp"
#include "youbot_lidar_nav/utils/logger.hpp"
#include "youbot_lidar_nav/utils/parameter_server.hpp"
#include "youbot_lidar_nav/utils/task_pool.hpp"

#include <chrono>
#include <csignal>
#include <filesystem>
#include <iostream>
#include <semaphore>
#include <thread>
#include <vector>

using namespace ybotln;

std::binary_semaphore signal_smph(0);
void sigint_handler(int signal)
{
    LOGGER_STREAM(MSG_LVL::INFO, "Caught terminal interrupt signal. Stopping...");
    signal_smph.release();
}

void default_parameters()
{
    PARAMETERS.set<bool>("logger/debug", false);
    PARAMETERS.set<bool>("logger/save", false);
    PARAMETERS.set<double>("driver/max_leaner_vel", 0.5);
    PARAMETERS.set<int>("tcp_server/port", 10000);
    PARAMETERS.set<int>("tcp_server/timeout", 500);
    PARAMETERS.set<std::string>("lidar/device_name", "/dev/ttyACM0");
    PARAMETERS.set<int>("lidar/baudrate", 115200);
}

void load_config()
{
    std::string config_path = "config/youbot_config.yaml";

    LOGGER_STREAM(MSG_LVL::INFO, "Loading parameters from file \"" << config_path << "\"...");

    YAML::Node config = YAML::LoadFile(config_path);

    PARAMETERS.set("logger/debug", config["logger"]["debug"].as<bool>());
    PARAMETERS.set("logger/save", config["logger"]["save"].as<bool>());
    PARAMETERS.set("driver/max_leaner_vel", config["driver"]["max_leaner_vel"].as<double>());
    PARAMETERS.set("tcp_server/port", config["tcp_server"]["port"].as<int>());
    PARAMETERS.set("tcp_server/timeout", config["tcp_server"]["timeout"].as<int>());
    PARAMETERS.set("tcp_server/port", config["tcp_server"]["port"].as<int>());
    PARAMETERS.set("tcp_server/timeout", config["tcp_server"]["timeout"].as<int>());
    PARAMETERS.set("lidar/device_name", config["lidar"]["device_name"].as<std::string>());
    PARAMETERS.set("lidar/baudrate", config["lidar"]["baudrate"].as<int>());

    LOGGER_STREAM(MSG_LVL::INFO, "Loading parameters completed successfully");
}

int main()
{
    LOGGER_STREAM(MSG_LVL::INFO, "Start initialization...");

    signal(SIGINT, sigint_handler);

    auto source_dir = std::filesystem::path(std::string(SOURCE_DIR));
    if (std::filesystem::exists(source_dir))
        std::filesystem::current_path(source_dir);

    default_parameters();

    try
    {
        load_config();
    }
    catch (const std::runtime_error& e)
    {
        LOGGER_STREAM(MSG_LVL::ERROR, "Failed to load parameters! " << e.what());
    }

    Logger::get_logger().set_debug(PARAMETERS.get<bool>("logger/debug"));
    Logger::get_logger().set_save(PARAMETERS.get<bool>("logger/save"));

    TaskPool task_pool;

    auto driver_task = std::make_unique<DriverTask>("driver");
    auto tcp_server_task = std::make_unique<TcpServerTask>("tcp_server");
    auto lidar_task = std::make_unique<LidarTask>("lidar");
    auto planner_task = std::make_unique<PlannerTask>("planner");
    task_pool.add_task(std::move(driver_task));
    task_pool.add_task(std::move(tcp_server_task));
    task_pool.add_task(std::move(lidar_task));
    task_pool.add_task(std::move(planner_task));

    task_pool.start_all();

    signal_smph.acquire();

    task_pool.stop_all();

    LOGGER_STREAM(MSG_LVL::INFO, "All threads stopped. Exiting program.");

    return 0;
}