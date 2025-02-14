#include "youbot_lidar_nav/tasks/streamer_task.hpp"

#include "youbot_lidar_nav/utils/logger.hpp"
#include "youbot_lidar_nav/utils/parameter_server.hpp"

namespace ybotln {

StreamerTask::StreamerTask(std::string name) : Task(name)
{
    update_parameters();
}

void StreamerTask::update_parameters()
{
    StreamSettings settings;
    settings.camera_dev = PARAMETERS.get<int>("streamer/camera_dev");
    settings.server_ip = iptoint(PARAMETERS.get<std::string>("streamer/server_ip"));
    settings.server_port = PARAMETERS.get<int>("streamer/port");
    settings.encode_quality = PARAMETERS.get<int>("streamer/quality");
    settings.frame_interval = PARAMETERS.get<int>("streamer/interval");
    settings.pack_size = PARAMETERS.get<int>("streamer/pack_size");
    settings.delay = PARAMETERS.get<int>("streamer/delay");
    settings.stream_switch = PARAMETERS.get<bool>("streamer/stream_switch");
    set_settings(settings);
}

void StreamerTask::open_camera(const int& camera_dev)
{
    if (!camera_cap.open(camera_dev))
    {
        stream_switch = false;
        LOGGER_STREAM(MSG_LVL::ERROR, "Couldn't start the camera: " << camera_dev << "!");
    }
}

void StreamerTask::set_settings(const StreamSettings& settings)
{
    video_transmitter.set_socket(inttoip(settings.server_ip), settings.server_port);
    video_transmitter.set_encode_quality(settings.encode_quality);
    video_transmitter.set_interval(settings.frame_interval);
    video_transmitter.set_pack_size(settings.pack_size);
    delay = std::chrono::milliseconds(settings.delay);
    stream_switch = settings.stream_switch;

    if (settings.camera_dev != stream_settings.camera_dev || !camera_cap.isOpened())
        open_camera(static_cast<int>(settings.camera_dev));

    stream_settings = settings;
}

void StreamerTask::task()
{
    while (!stop_flag)
    {
        if (!camera_processing())
        {
            // In case of an error receiving data from the camera,
            // the thread goes to sleep until new commands are received.
            try_process_commands_for(std::chrono::milliseconds(100));
        }
        try_process_commands_for(delay);
    }
}

bool StreamerTask::camera_processing()
{
    if (!stream_switch)
        return false;

    cv::Mat src;

    if (!camera_cap.read(src))
    {
        LOGGER_STREAM(MSG_LVL::ERROR, "Read camera error!");
        return false;
    }

    if (src.empty())
    {
        LOGGER_STREAM(MSG_LVL::ERROR, "Camera no data!");
        return false;
    }

    video_transmitter.send_img(src);
    return true;
}

std::string StreamerTask::inttoip(const uint32_t& src) const
{
    char ip_str[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &src, ip_str, INET_ADDRSTRLEN);
    return std::string(ip_str);
}

uint32_t StreamerTask::iptoint(const std::string& src) const
{
    uint32_t res;
    inet_pton(AF_INET, src.c_str(), &res);
    return res;
}

/**************************** Command interface ****************************/

void SettingsCommand::execute(Task& task)
{
    StreamerTask& streamer_task = dynamic_cast<StreamerTask&>(task);
    streamer_task.set_settings(settings_);
    result_ = true;
}

void SettingsCommand::set_settings(const StreamSettings& settings)
{
    settings_ = settings;
}

}  // namespace ybotln
