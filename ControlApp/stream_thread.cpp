#include "stream_thread.h"
#include "QDebug"
#include "udp_streamer/udp_streamer.hpp"

StreamThread::StreamThread(QObject *parent)
    : QThread(parent)
{}

StreamThread::~StreamThread()
{
    this->requestInterruption();
    this->quit();
    this->wait();
}

/*********************************** Slots ***********************************/

void StreamThread::startStream(const quint16 &port)
{
    if (this->isRunning())
        return;
    cameraUdpReceiver = std::make_unique<udp_streamer::Receiver>();
    bindPort(port);
    this->start();
}

void StreamThread::stopStream()
{
    this->requestInterruption();
}

/*********************************** Other ***********************************/

void StreamThread::run()
{
    StreamThread::from_stream();

    cameraUdpReceiver.reset(nullptr);
}

void StreamThread::bindPort(const quint16 &port)
{
    cameraUdpReceiver->set_socket("", port);
    cameraUdpReceiver->socket_bind();
}

void StreamThread::from_stream()
{
    cv::Mat output_img;

    while (!isInterruptionRequested()) {
        cameraUdpReceiver->receive_img(output_img);

        if (output_img.empty()) {
            qDebug() << "Failed to capture frame from receiver";
            this->msleep(200);
            continue;
        }

        QImage qImage(output_img.data, output_img.cols, output_img.rows, QImage::Format_RGB888);
        qImage = qImage.rgbSwapped();
        emit frameAvailable(qImage);
    }
}
