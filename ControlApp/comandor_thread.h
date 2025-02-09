#ifndef COMANDOR_THREAD_H
#define COMANDOR_THREAD_H

#include <QTcpSocket>
#include <QTimer>
#include <QThread>
#include <vector>
#include <QMessageBox>

#include "protocol.h"
#include "qthread.h"

struct RouteStep
{
    double longitudinal_vel = 0; // m/s
    double transversal_vel = 0;  // m/s
    double angular_vel = 0;      // rad/s
    int duration;                // ms
};

class ComandorThread : public QThread
{
    Q_OBJECT
public:
    ComandorThread(QObject *parent = nullptr);
    ~ComandorThread();

    void sendRoute(const std::vector<RouteStep> &route_list);
    void sendStop();
    void sendStreamSettings(const StreamSettings &settings);

public slots:
    void connectHost(const QString &host_ip, const quint16 &port);
    void disconnectHost();

signals:
    void netError(QAbstractSocket::SocketError socketError);
    void disconnected();
    void connected();
    void requestControl();

private:
    void sendTcp(const size_t msg_size = TX_MSG_SIZE);
    void sendRouteMsg(const std::vector<RouteStep>::const_iterator begin,
                      const std::vector<RouteStep>::const_iterator end,
                      bool reset);

private:
    QTcpSocket *tcpSocket = nullptr;
    QTimer *tcpResendTimer = nullptr;
    const int resendTime = 500; // ms
    std::vector<uint8_t> txBuffer;
    std::vector<uint8_t> rxBuffer;
};

#endif // COMANDOR_THREAD_H
