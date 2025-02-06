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
    rate = std::chrono::milliseconds(PARAMETERS.get<int>("streamer/rate"));

    std::string camera_dev = PARAMETERS.get<std::string>("streamer/camera_dev");
    open_camera(camera_dev);

    StreamSettings settings;
    settings.server_ip = PARAMETERS.get<std::string>("streamer/server_ip");
    settings.server_port = PARAMETERS.get<int>("streamer/port");
    settings.quality = PARAMETERS.get<int>("streamer/quality");
    settings.interval = PARAMETERS.get<int>("streamer/interval");
    settings.pack_size = PARAMETERS.get<int>("streamer/pack_size");
    settings.width = PARAMETERS.get<int>("streamer/width");
    settings.height = PARAMETERS.get<int>("streamer/height");
    set_streamer(settings);
}

void StreamerTask::open_camera(const std::string& camera_dev)
{
    if (!camera_cap.open(camera_dev))
    {
        LOGGER_STREAM(MSG_LVL::ERROR, "Couldn't start the camera: " << camera_dev << "!");
    }
}

void StreamerTask::set_streamer(const StreamSettings& settings)
{
    video_transmitter.set_socket(settings.server_ip, settings.server_port);
    video_transmitter.set_encode_quality(settings.quality);
    video_transmitter.set_interval(settings.interval);
    video_transmitter.set_pack_size(settings.pack_size);
    video_transmitter.set_size(settings.width, settings.height);
}

void StreamerTask::task()
{
    while (!stop_flag)
    {
        if (!camera_processing())
        {
            // In case of an error receiving data from the camera, 
            // the thread goes to sleep until new commands are received.
            try_process_commands();
        }
        try_process_commands_for(rate);
    }
}

bool StreamerTask::camera_processing()
{
    if (stream_switch)
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

}  // namespace ybotln
