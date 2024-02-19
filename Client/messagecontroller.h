#ifndef MESSAGECONTROLLER_H
#define MESSAGECONTROLLER_H

#include <QByteArray>
#include <QDebug>
#include <QQueue>
#include <QList>

#define REQUEST_SUCCESS "ok"
#define REQUEST_FAIL "fail"

struct request
{
    enum class type
    {
        start_measure = 0x00,
        set_range     = 0x01,
        stop_measure  = 0x02,
        get_status    = 0x03,
        get_result    = 0x04
    };

    inline static QString get_type_str(type _type)
    {
        switch (_type)
        {
            case type::start_measure:
                return "start_measure";
            case type::set_range:
                return "set_range";
            case type::stop_measure:
                return "stop_measure";
            case type::get_status:
                return "get_status";
            case type::get_result:
                return "get_result";
            default:
                break;
        }
        return "unknown";
    }

    type measure_request;
    QString channelName;
};

struct response
{
    bool success;
    QString params;
    request _request;
};

enum class request_type
{
    idle_state    = 0x00,
    measure_state = 0x01,
    busy_state    = 0x02,
    error_state   = 0xff
};

class MessageController : public QObject
{
    Q_OBJECT
public:
    MessageController();

    void handleResponse(QList<QByteArray>);

    QString addRequest(request::type, QString channelName, QList<QString> = QList<QString>());

signals:
    void signalMessageInfo(response);

public slots:
    void handleMessage(QByteArray);

private:
    QQueue<request> requestQueue;
};

#endif  // MESSAGECONTROLLER_H
