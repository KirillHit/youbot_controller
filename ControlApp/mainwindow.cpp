#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    buttonInit();
    sliderInit();
    uiValidator();
    comandorInit();
}

MainWindow::~MainWindow()
{
    delete ui;
}

/******************************** Init methods ********************************/

void MainWindow::sliderInit()
{
    ui->labelLinVel->setText(QString::number(ui->sliderLinVel->value() / 100.0, 'f', 2));
    ui->lableAngVel->setText(QString::number(ui->sliderAngVel->value() / 100.0, 'f', 2));

    connect(ui->sliderLinVel, &QAbstractSlider::valueChanged, this, &MainWindow::sliderHandle);
    connect(ui->sliderAngVel, &QAbstractSlider::valueChanged, this, &MainWindow::sliderHandle);
}

void MainWindow::buttonInit()
{
    connect(ui->butLeftForward, &QPushButton::pressed, this, &MainWindow::buttonHandle);
    connect(ui->butLeft, &QPushButton::pressed, this, &MainWindow::buttonHandle);
    connect(ui->butLeftBack, &QPushButton::pressed, this, &MainWindow::buttonHandle);
    connect(ui->butForward, &QPushButton::pressed, this, &MainWindow::buttonHandle);
    connect(ui->butStop, &QPushButton::pressed, this, &MainWindow::buttonHandle);
    connect(ui->butBack, &QPushButton::pressed, this, &MainWindow::buttonHandle);
    connect(ui->butRightForward, &QPushButton::pressed, this, &MainWindow::buttonHandle);
    connect(ui->butRight, &QPushButton::pressed, this, &MainWindow::buttonHandle);
    connect(ui->butRightBack, &QPushButton::pressed, this, &MainWindow::buttonHandle);
    connect(ui->butRotLeft, &QPushButton::pressed, this, &MainWindow::buttonHandle);
    connect(ui->butRotRight, &QPushButton::pressed, this, &MainWindow::buttonHandle);

    connect(ui->butLeftForward, &QPushButton::released, this, &MainWindow::buttonHandle);
    connect(ui->butLeft, &QPushButton::released, this, &MainWindow::buttonHandle);
    connect(ui->butLeftBack, &QPushButton::released, this, &MainWindow::buttonHandle);
    connect(ui->butForward, &QPushButton::released, this, &MainWindow::buttonHandle);
    connect(ui->butStop, &QPushButton::released, this, &MainWindow::buttonHandle);
    connect(ui->butBack, &QPushButton::released, this, &MainWindow::buttonHandle);
    connect(ui->butRightForward, &QPushButton::released, this, &MainWindow::buttonHandle);
    connect(ui->butRight, &QPushButton::released, this, &MainWindow::buttonHandle);
    connect(ui->butRightBack, &QPushButton::released, this, &MainWindow::buttonHandle);
    connect(ui->butRotLeft, &QPushButton::released, this, &MainWindow::buttonHandle);
    connect(ui->butRotRight, &QPushButton::released, this, &MainWindow::buttonHandle);
}

void MainWindow::uiValidator()
{
    QString IpRange = "(?:[0-1]?[0-9]?[0-9]|2[0-4][0-9]|25[0-5])";
    QRegularExpression IpRegex("^" + IpRange + "(\\." + IpRange + ")" + "(\\." + IpRange + ")" +
                               "(\\." + IpRange + ")$");
    QRegularExpressionValidator *ipValidator = new QRegularExpressionValidator(IpRegex, this);
    ui->lineIp->setValidator(ipValidator);

    ui->linePort->setValidator(new QIntValidator(1024, 65535, this));
}

void MainWindow::comandorInit()
{
    connect(&comandorThread, &ComandorThread::netError, this, &MainWindow::displayNetError);
    connect(&comandorThread, &ComandorThread::connected, this, &MainWindow::connectedHandle);
    connect(&comandorThread, &ComandorThread::disconnected, this, &MainWindow::disconnectedHandle);

    connect(ui->butConnect, &QPushButton::pressed, this, [=]() {
        this->comandorThread.connectHost(ui->lineIp->text(), ui->linePort->text().toInt());
    });
    connect(ui->butDisconnect,
            &QPushButton::pressed,
            &comandorThread,
            &ComandorThread::disconnectHost);
    connect(&comandorThread, &ComandorThread::requestControl, this, &MainWindow::buttonHandle);
}

/*********************************** Slots ***********************************/

void MainWindow::displayNetError(QAbstractSocket::SocketError socketError)
{
    switch (socketError)
    {
    case QAbstractSocket::RemoteHostClosedError:
        break;
    case QAbstractSocket::HostNotFoundError:
        QMessageBox::information(this, tr("Tcp Client"),
                                 tr("The host was not found. Please check the "
                                    "host name and port settings."));
        break;
    case QAbstractSocket::ConnectionRefusedError:
        QMessageBox::information(this, tr("Tcp Client"),
                                 tr("The connection was refused by the peer. "
                                    "Make sure the server is running, "
                                    "and check that the ip address "
                                    "settings are correct."));
        break;
    default:
        QMessageBox::information(this,
                                 tr("Tcp Client"),
                                 tr("The following error occurred: %1. See "
                                    "https://doc.qt.io/qt-6/qabstractsocket.html#SocketError-enum")
                                     .arg(static_cast<int>(socketError)));
    }

    ui->butConnect->setEnabled(true);
}

void MainWindow::disconnectedHandle()
{
    ui->butConnect->setEnabled(true);
    ui->butConnect->setDown(false);
    ui->butDisconnect->setEnabled(false);
    ui->butDisconnect->setDown(true);
}

void MainWindow::connectedHandle()
{
    ui->butConnect->setEnabled(false);
    ui->butConnect->setDown(true);
    ui->butDisconnect->setEnabled(true);
    ui->butDisconnect->setDown(false);
}

/********************************* UI handle *********************************/

void MainWindow::sliderHandle(int value)
{
    QObject *pObject = sender();

    if (pObject == ui->sliderLinVel)
    {
        ui->labelLinVel->setText(QString::number(value / 100.0, 'f', 2));
    }
    else if (pObject == ui->sliderAngVel)
    {
        ui->lableAngVel->setText(QString::number(value / 100.0, 'f', 2));
    }
}

void MainWindow::buttonHandle()
{
    std::vector<RouteStep> route(1);
    route[0].duration = routeResolution;
    if (!ui->butStop->isDown())
    {
        route[0].longitudinal_vel =
            static_cast<double>(
                ui->sliderLinVel->value() *
                (static_cast<int>(ui->butForward->isDown() || ui->butLeftForward->isDown() ||
                                  ui->butRightForward->isDown()) -
                 static_cast<int>(ui->butBack->isDown() || ui->butLeftBack->isDown() ||
                                  ui->butRightBack->isDown()))) /
            100.0;

        route[0].transversal_vel =
            static_cast<double>(
                ui->sliderLinVel->value() *
                (static_cast<int>(ui->butLeft->isDown() || ui->butLeftBack->isDown() ||
                                  ui->butLeftForward->isDown()) -
                 static_cast<int>(ui->butRight->isDown() || ui->butRightBack->isDown() ||
                                  ui->butRightForward->isDown()))) /
            100.0;

        route[0].angular_vel = static_cast<double>(ui->sliderAngVel->value() *
                                                   (static_cast<int>(ui->butRotLeft->isDown()) -
                                                    static_cast<int>(ui->butRotRight->isDown()))) /
                               100.0;
    }
    comandorThread.sendRoute(route);
}

void MainWindow::keyPressEvent(QKeyEvent *keyEvent)
{
    if (keyEvent->isAutoRepeat())
    {
        keyEvent->accept();
        return;
    }

    switch (keyEvent->key())
    {
    case Qt::Key_Q:
        pressBut(ui->butLeftForward);
        break;
    case Qt::Key_W:
        pressBut(ui->butForward);
        break;
    case Qt::Key_E:
        pressBut(ui->butRightForward);
        break;
    case Qt::Key_A:
        pressBut(ui->butLeft);
        break;
    case Qt::Key_S:
        pressBut(ui->butStop);
        break;
    case Qt::Key_D:
        pressBut(ui->butRight);
        break;
    case Qt::Key_Z:
        pressBut(ui->butLeftBack);
        break;
    case Qt::Key_X:
        pressBut(ui->butBack);
        break;
    case Qt::Key_C:
        pressBut(ui->butRightBack);
        break;
    case Qt::Key_Comma:
        pressBut(ui->butRotLeft);
        break;
    case Qt::Key_Period:
        pressBut(ui->butRotRight);
        break;
    default:
        break;
    }

    keyEvent->accept();
}

void MainWindow::keyReleaseEvent(QKeyEvent *keyEvent)
{
    if (keyEvent->isAutoRepeat())
    {
        keyEvent->accept();
        return;
    }

    switch (keyEvent->key())
    {
    case Qt::Key_Q:
        releasBut(ui->butLeftForward);
        break;
    case Qt::Key_W:
        releasBut(ui->butForward);
        break;
    case Qt::Key_E:
        releasBut(ui->butRightForward);
        break;
    case Qt::Key_A:
        releasBut(ui->butLeft);
        break;
    case Qt::Key_S:
        releasBut(ui->butStop);
        break;
    case Qt::Key_D:
        releasBut(ui->butRight);
        break;
    case Qt::Key_Z:
        releasBut(ui->butLeftBack);
        break;
    case Qt::Key_X:
        releasBut(ui->butBack);
        break;
    case Qt::Key_C:
        releasBut(ui->butRightBack);
        break;
    case Qt::Key_Comma:
        releasBut(ui->butRotLeft);
        break;
    case Qt::Key_Period:
        releasBut(ui->butRotRight);
        break;
    default:
        break;
    }

    keyEvent->accept();
}

void MainWindow::pressBut(QPushButton *button)
{
    button->setDown(true);
    emit button->pressed();
}

void MainWindow::releasBut(QPushButton *button)
{
    button->setDown(false);
    emit button->released();
}
