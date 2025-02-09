#ifndef STREAM_THREAD_H
#define STREAM_THREAD_H

#include <QImage>
#include <QThread>
#include <memory>
#include <opencv2/opencv.hpp>

#include "udp_streamer/udp_streamer.hpp"

class StreamThread : public QThread
{
    Q_OBJECT
public:
    explicit StreamThread(QObject *parent = nullptr);
    ~StreamThread();

public slots:
    void startStream(const quint16 &port);
    void stopStream();

signals:
    void frameAvailable(const QImage& frame);

protected:
    void run() override;

private:
    void from_stream();
    void bindPort(const quint16 &port);

private:
    std::unique_ptr<udp_streamer::Receiver> cameraUdpReceiver;
};


#endif // STREAM_THREAD_H
