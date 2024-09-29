#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QDebug>
#include <QIntValidator>
#include <QKeyEvent>
#include <QLineEdit>
#include <QMainWindow>
#include <QMessageBox>
#include <QPushButton>
#include <QRandomGenerator>
#include <QTcpSocket>
#include <QTimer>
#include <vector>

#include "planner.h"
#include "protocol.h"

QT_BEGIN_NAMESPACE
namespace Ui
{
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

  public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

  protected:
    void keyPressEvent(QKeyEvent *keyEvent) override;
    void keyReleaseEvent(QKeyEvent *keyEvent) override;

    void pressBut(QPushButton *button);
    void releasBut(QPushButton *button);
    void buttonInit();
    void buttonHandle();
    void buttonRandHandle();

    void sliderInit();
    void sliderHandle(int value);

    void uiValidator();

    void requestNewConnection();
    void disconnect();
    void sendTcp(const size_t msg_size = TX_MSG_SIZE);
    void displayNetError(QAbstractSocket::SocketError socketError);
    void disconnectedHandle();

    void sendRoute(const std::vector<RouteStep> &route_list);

  private:
    Ui::MainWindow *ui;
    QTcpSocket *tcpSocket = nullptr;
    QTimer *tcpResendTimer = nullptr;
    const int resendTime = 400;      // ms
    const int routeResolution = 500; // ms
    std::vector<uint8_t> txBuffer;
    std::vector<uint8_t> rxBuffer;
};

#endif // MAINWINDOW_H
