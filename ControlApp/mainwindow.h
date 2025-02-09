#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QDebug>
#include <QFormLayout>
#include <QIntValidator>
#include <QKeyEvent>
#include <QLineEdit>
#include <QMainWindow>
#include <QMessageBox>
#include <QPushButton>
#include <QRadioButton>
#include <QRandomGenerator>
#include <QTcpSocket>
#include <QTimer>
#include <vector>

#include "comandor_thread.h"
#include "protocol.h"
#include "stream_thread.h"

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

public slots:
    void displayNetError(QAbstractSocket::SocketError socketError);
    void disconnectedHandle();
    void connectedHandle();

signals:
    void requestNewConnection();

private:
    void buttonInit();
    void sliderInit();
    void uiValidator();
    void comandorInit();
    void streamInit();

    void keyPressEvent(QKeyEvent *keyEvent) override;
    void keyReleaseEvent(QKeyEvent *keyEvent) override;
    void pressBut(QPushButton *button);
    void releasBut(QPushButton *button);
    void buttonHandle();
    void sliderHandle(int value);
    void streamSettingsHandle();

    std::string inttoip(const uint32_t &src) const;
    uint32_t iptoint(const std::string &src) const;

private:
    Ui::MainWindow *ui;
    ComandorThread comandorThread;
    StreamThread streamThread;
    QDialog *streamDlg = nullptr;
    StreamSettings streamSettings;
    const int routeResolution = 600; // ms
};

#endif // MAINWINDOW_H
