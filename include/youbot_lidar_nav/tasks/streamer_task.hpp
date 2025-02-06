#ifndef TCP_SERVER_TASK_HPP
#define TCP_SERVER_TASK_HPP

#include "udp_streamer/udp_streamer.hpp"
#include "youbot_lidar_nav/utils/task_pool.hpp"

#include <chrono>
#include <string>

namespace ybotln {

struct StreamSettings
{
    std::string server_ip;
    int server_port;
    int quality;
    int interval;
    int pack_size;
    int width;
    int height;
};

class StreamerTask : public Task
{
public:
    StreamerTask(std::string name);
    ~StreamerTask() = default;
    void update_parameters();

private:
    void task() override;
    void open_camera(const std::string& camera_dev);
    void set_streamer(const StreamSettings& settings);
    bool camera_processing();

    udp_streamer::Transmitter video_transmitter;
    cv::VideoCapture camera_cap;
    std::chrono::milliseconds rate;
    bool stream_switch = true;
};

}  // namespace ybotln

#endif
