#include "messagecontroller.h"

#include <QMetaType>

MessageController::MessageController()
{
    qRegisterMetaType<response>("response");
}

void MessageController::handleMessage(QByteArray arr)
{
    if (!arr.endsWith('\n'))
    {
        qWarning() << "invalid message" << arr;
        return;
    }
    arr.replace(" ", "");
    arr.replace("\n", "");
    auto parts = arr.split(',');

    if (parts.empty())
    {
        qWarning() << "message empty";
        return;
    }

    if (!requestQueue.empty())
        handleResponse(parts);
}

void MessageController::handleResponse(QList<QByteArray> list)
{
    auto req    = requestQueue.dequeue();
    auto okFail = list.at(0);
    list.removeFirst();
    qInfo() << "handleResponse" << okFail << request::get_type_str(req.measure_request) << req.channelName << list;
    switch (req.measure_request)
    {
        case request::type::start_measure: {
            if (okFail == REQUEST_SUCCESS)
                emit signalMessageInfo(response{ true, "", req });
            else if (okFail == REQUEST_FAIL)
                emit signalMessageInfo(response{ false, "", req });
            break;
        }
        case request::type::set_range: {
            if (list.size() == 2)
            {
                if (okFail == REQUEST_SUCCESS)
                    emit signalMessageInfo(response{ true, "", req });
                else if (okFail == REQUEST_FAIL)
                    emit signalMessageInfo(response{ false, "", req });
                else
                    qWarning() << "Error set_range message";
            }
            break;
        }
        case request::type::stop_measure: {
            if (okFail == REQUEST_SUCCESS)
                emit signalMessageInfo(response{ true, "", req });
            else if (okFail == REQUEST_FAIL)
                emit signalMessageInfo(response{ false, "", req });
            break;
        }
        case request::type::get_status: {
            if (okFail == REQUEST_SUCCESS)
            {
                qInfo() << "list size" << list.size();
                if (!list.empty())
                {
                    qInfo() << "list at 0" << list.at(0);
                    emit signalMessageInfo(response{ true, list.at(0), req });
                }
            }
            if (okFail == REQUEST_FAIL)
                emit signalMessageInfo(response{ false, "", req });
            break;
        }
        case request::type::get_result:
            break;
        default:
            break;
    }
}

QString MessageController::addRequest(request::type _type, QString channelName, QList<QString> params)
{
    requestQueue.enqueue({ _type, channelName });

    QString req = request::get_type_str(_type) + " " + channelName;
    if (!params.empty())
        for (const auto& iter : params)
            req += ", " + iter;

    return req + '\n';
}
