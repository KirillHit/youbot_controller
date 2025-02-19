#ifndef STREAMER_TASK_HPP
#define STREAMER_TASK_HPP

#include "udp_streamer/udp_streamer.hpp"
#include "youbot_lidar_nav/protocol.hpp"
#include "youbot_lidar_nav/utils/task_pool.hpp"

#include <chrono>
#include <string>

namespace ybotln {

class StreamerTask : public Task
{
public:
    StreamerTask(std::string name);
    ~StreamerTask() = default;
    void update_parameters();
    void set_settings(const StreamSettings& settings);

private:
    void task() override;
    void open_camera(const int& camera_dev);
    bool camera_processing();

    std::string inttoip(const uint32_t& src) const;
    uint32_t iptoint(const std::string& src) const;

    udp_streamer::Transmitter video_transmitter;
    cv::VideoCapture camera_cap;
    StreamSettings stream_settings;
    std::chrono::milliseconds delay;
    bool stream_switch = true;
};

class SettingsCommand : public Command
{
public:
    SettingsCommand() = default;
    void execute(Task& task) override;
    void set_settings(const StreamSettings& settings);

private:
    StreamSettings settings_;
};

}  // namespace ybotln

#endif
