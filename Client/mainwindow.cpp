#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget* parent)
  : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    initUI();

    qRegisterMetaType<response>("response");

    connect(&m_socketController, &SocketController::connected, this, &MainWindow::connected);
    connect(&m_socketController, &SocketController::disconnected, this, &MainWindow::disconnected);
    connect(&m_socketController, &SocketController::messageReceived, &m_messageController, &MessageController::handleMessage);
    connect(&m_messageController, &MessageController::signalMessageInfo, this, &MainWindow::handleMessage);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::initUI()
{
    setFixedSize(800, 300);
    ui->pushButtonConnectServer->setStyleSheet("background-color: red;");
}

QString MainWindow::readLineEdit(QLineEdit* lineEdit)
{
    if (!lineEdit)
        return "";

    auto text = lineEdit->text();
    if (text.isEmpty())
        return "";

    return text;
}

void MainWindow::on_pushButtonConnectServer_clicked()
{
    if (m_socketController.getConnectionState() == server_connection_state::disconnect)
    {
        ui->pushButtonConnectServer->setText("Connecting...");
        ui->pushButtonConnectServer->setStyleSheet("background-color: yellow;");
        m_socketController.startConnectToServer();
    }
    //    else if (m_socketController.getConnectionState() == server_connection_state::connection)
    //    {
    //        m_socketController.stopConnectToServer();
    //        ui->pushButtonConnectServer->setText("Connect");
    //        ui->pushButtonConnectServer->setStyleSheet("background-color: red;");
    //    }
    //    else if (m_socketController.getConnectionState() == server_connection_state::connected)
    //    {
    //        ui->pushButtonConnectServer->setText("Disconnection...");
    //        ui->pushButtonConnectServer->setStyleSheet("background-color: green;");
    //        m_socketController.disconnectFromServer();
    //    }
}

void MainWindow::connected()
{
    ui->pushButtonConnectServer->setText("Connected");
    //    ui->pushButtonConnectServer->setText("Disconnect");
    ui->pushButtonConnectServer->setStyleSheet("background-color: green;");
}

void MainWindow::disconnected()
{
    ui->pushButtonConnectServer->setText("Connect");
    ui->pushButtonConnectServer->setStyleSheet("background-color: red;");
}

void MainWindow::handleMessage(response new_response)
{
    if (new_response.success)
    {
        qInfo() << "new success response" << new_response.params;
        if (!new_response.params.isEmpty())
        {
            if (new_response._request.measure_request == request::type::get_status)
            {
                QString statusString("Status:" + new_response.params);
                ui->labelStatus->setText(statusString);
            }
        }
    }
    else
        qInfo() << "new fail response";
}

void MainWindow::on_pushButtonGetChannelStatus_clicked()
{
    auto channelName = readLineEdit(ui->lineEditChannelName);
    if (channelName.isEmpty())
        return;

    m_socketController.sendMessage(m_messageController.addRequest(request::type::get_status, channelName));
}

void MainWindow::on_pushButtonStartChannel_clicked()
{
    auto channelName = readLineEdit(ui->lineEditChannelName);
    if (channelName.isEmpty())
        return;

    m_socketController.sendMessage(m_messageController.addRequest(request::type::start_measure, channelName));
}

void MainWindow::on_pushButtonStopChannel_clicked()
{
    auto channelName = readLineEdit(ui->lineEditChannelName);
    if (channelName.isEmpty())
        return;

    m_socketController.sendMessage(m_messageController.addRequest(request::type::stop_measure, channelName));
}

void MainWindow::on_pushButtonGetResult_clicked()
{
    auto channelName = readLineEdit(ui->lineEditChannelName);
    if (channelName.isEmpty())
        return;

    m_socketController.sendMessage(m_messageController.addRequest(request::type::get_result, channelName));
}

void MainWindow::on_pushButtonSetRange_clicked()
{
    auto channelName = readLineEdit(ui->lineEditChannelName);
    if (channelName.isEmpty())
        return;

    auto channelRange = readLineEdit(ui->lineEditChannelRange);
    if (channelRange.isEmpty())
        return;

    m_socketController.sendMessage(m_messageController.addRequest(request::type::set_range, channelName, QList<QString>({ channelRange })));
}
