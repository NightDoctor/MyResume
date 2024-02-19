#ifndef SOCKETCONTROLLER_H
#define SOCKETCONTROLLER_H

#include <QObject>
#include <QTcpSocket>
#include <QHostAddress>
#include <QTimer>

enum class server_connection_state
{
    disconnect = 0x00,
    connection = 0x01,
    connected  = 0x02,
};

class SocketController : public QObject
{
    Q_OBJECT
public:
    explicit SocketController(QObject* parent = nullptr);
    ~SocketController();

    inline server_connection_state getConnectionState() const
    {
        return connection_state;
    }

public slots:
    void connectToServer();
    void ping();
    void startConnectToServer();
    void stopConnectToServer();
    void disconnectFromServer();
    void sendMessage(const QString& message);

signals:
    void connected();
    void disconnected();
    void messageReceived(QByteArray message);
    void errorOccurred(const QString& error);

private slots:
    void onConnected();
    void onDisconnected();
    void onReadyRead();
    void onError(QAbstractSocket::SocketError socketError);

private:
    server_connection_state connection_state;
    QTimer* timerConnectToServer;
    QTimer* timerPing;
    QTcpSocket* m_socket;
};

#endif  // SOCKETCONTROLLER_H
