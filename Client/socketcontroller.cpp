#include "socketcontroller.h"

constexpr char k_hostIp[] = "192.168.159.129";
constexpr uint32_t port   = 5678;

SocketController::SocketController(QObject* parent)
  : QObject(parent), connection_state(server_connection_state::disconnect), timerConnectToServer(new QTimer(this)), timerPing(new QTimer(this)), m_socket(new QTcpSocket(this))
{
    connect(m_socket, &QTcpSocket::connected, this, &SocketController::onConnected);
    connect(m_socket, &QTcpSocket::disconnected, this, &SocketController::onDisconnected);
    connect(m_socket, &QTcpSocket::readyRead, this, &SocketController::onReadyRead);
    connect(m_socket, &QTcpSocket::errorOccurred, this, &SocketController::onError);

    connect(timerConnectToServer, &QTimer::timeout, this, &SocketController::connectToServer);
    connect(timerPing, &QTimer::timeout, this, &SocketController::ping);

    timerConnectToServer->setInterval(1000);
    timerPing->setInterval(1000);
}

SocketController::~SocketController()
{
    if (m_socket)
    {
        if (m_socket->isOpen())
            m_socket->disconnect();
        delete m_socket;
    }
    if (timerConnectToServer)
        timerConnectToServer->deleteLater();
}

void SocketController::startConnectToServer()
{
    connection_state = server_connection_state::connection;
    timerConnectToServer->start();
}

void SocketController::connectToServer()
{
    m_socket->connectToHost(k_hostIp, port);
}

void SocketController::ping()
{
    sendMessage("ping\n");
}

void SocketController::stopConnectToServer()
{
    timerConnectToServer->stop();
    connection_state = server_connection_state::disconnect;
}

void SocketController::disconnectFromServer()
{
    if (m_socket)
        m_socket->disconnect();
    onDisconnected();
}

void SocketController::sendMessage(const QString& message)
{
    qInfo() << "send new message" << message;
    m_socket->write(message.toUtf8());
}

void SocketController::onConnected()
{
    qInfo() << "Connected";
    connection_state = server_connection_state::connected;
    timerConnectToServer->stop();
    timerPing->start();
    emit connected();
}

void SocketController::onDisconnected()
{
    qInfo() << "Disconnected";
    timerPing->stop();
    connection_state = server_connection_state::disconnect;
    emit disconnected();
}

void SocketController::onReadyRead()
{
    while (m_socket->bytesAvailable() > 0)
    {
        QByteArray data = m_socket->readAll();
        qInfo() << "new rx message" << data;
        emit messageReceived(data);
    }
}

void SocketController::onError(QAbstractSocket::SocketError socketError)
{
    Q_UNUSED(socketError)
    emit errorOccurred(m_socket->errorString());
}
