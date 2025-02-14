#include "comandor_thread.h"

ComandorThread::ComandorThread(QObject *parent)
    : QThread(parent)
    , tcpSocket(new QTcpSocket(this))
{
    txBuffer.resize(TX_MSG_SIZE, 0);
    rxBuffer.resize(RX_MSG_SIZE);

    tcpResendTimer = new QTimer(this);
    connect(tcpResendTimer, &QTimer::timeout, this, &ComandorThread::requestControl);
    tcpResendTimer->start(resendTime);

    connect(tcpSocket, &QAbstractSocket::errorOccurred, this, &ComandorThread::netError);
    connect(tcpSocket, &QAbstractSocket::disconnected, this, &ComandorThread::disconnected);
    connect(tcpSocket, &QAbstractSocket::connected, this, &ComandorThread::connected);
}

ComandorThread::~ComandorThread()
{
    disconnectHost();
}

/*********************************** Slots ***********************************/

void ComandorThread::connectHost(const QString &host_ip, const quint16 &port)
{
    tcpSocket->abort();
    tcpSocket->connectToHost(host_ip, port);
}

void ComandorThread::disconnectHost()
{
    sendStop();
    tcpSocket->waitForBytesWritten(1000);
    tcpSocket->abort();
}

/*********************************** Other ***********************************/

void ComandorThread::sendTcp(const size_t msg_size)
{
    if (tcpSocket->state() != QAbstractSocket::ConnectedState) {
        return;
    }
    tcpSocket->write(reinterpret_cast<char *>(txBuffer.data()), msg_size);
}

void ComandorThread::sendRouteMsg(const std::vector<RouteStep>::const_iterator begin,
                                  const std::vector<RouteStep>::const_iterator end,
                                  bool reset)
{
    ptrdiff_t num_step = std::distance(begin, end);
    if (num_step <= 0) {
        qWarning("Incorrect iterators");
        return;
    }
    txBuffer[0] = static_cast<uint8_t>(DataId::GO_ROUTE);
    RouteMsgHeader *route_header = reinterpret_cast<RouteMsgHeader *>(&txBuffer[1]);
    route_header->reset_route = reset;
    route_header->step_count = std::min(static_cast<size_t>(num_step), MAX_ROUTE_STEPS);

    RouteStepMsg *step_msg = reinterpret_cast<RouteStepMsg *>(&txBuffer[3]);
    auto step = begin;
    for (size_t step_idx = 0; step_idx < route_header->step_count; ++step_idx, ++step_msg, ++step) {
        step_msg->longitudinal_vel = static_cast<int16_t>(step->longitudinal_vel * 1000);
        step_msg->transversal_vel = static_cast<int16_t>(step->transversal_vel * 1000);
        step_msg->angular_vel = static_cast<int16_t>(step->angular_vel * 1000);
        step_msg->duration = static_cast<int16_t>(step->duration);
    }
    sendTcp();
}

void ComandorThread::sendRoute(const std::vector<RouteStep> &route_list)
{
    size_t num_msg = 1 + ((route_list.size() - 1) / MAX_ROUTE_STEPS);
    auto step = route_list.begin();
    for (size_t msg_idx = 0; msg_idx < num_msg; ++msg_idx, step += MAX_ROUTE_STEPS) {
        sendRouteMsg(step, route_list.end(), !static_cast<bool>(msg_idx));
    }
    tcpResendTimer->start(resendTime);
}

void ComandorThread::sendStop()
{
    std::vector<RouteStep> route_list(1);
    route_list[0].longitudinal_vel = 0;
    route_list[0].transversal_vel = 0;
    route_list[0].angular_vel = 0;
    route_list[0].duration = 0;
    sendRoute(route_list);
}

void ComandorThread::sendStreamSettings(const StreamSettings &settings)
{
    txBuffer[0] = static_cast<uint8_t>(DataId::SET_CAMERA);
    std::memcpy(&txBuffer[1], &settings, sizeof(StreamSettings));
    sendTcp();
}

bool ComandorThread::isConnected() const
{
    return tcpSocket->state() == QAbstractSocket::ConnectedState;
}
