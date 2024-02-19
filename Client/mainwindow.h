#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QHBoxLayout>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QStandardItemModel>
#include <QTableView>

#include "messagecontroller.h"
#include "socketcontroller.h"

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
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

    void initUI();

    QString readLineEdit(QLineEdit*);

private slots:
    void on_pushButtonConnectServer_clicked();
    void connected();
    void disconnected();

    void handleMessage(response);

    void on_pushButtonGetChannelStatus_clicked();

    void on_pushButtonStartChannel_clicked();

    void on_pushButtonStopChannel_clicked();

    void on_pushButtonGetResult_clicked();

    void on_pushButtonSetRange_clicked();

private:
    Ui::MainWindow* ui;

    SocketController m_socketController;
    MessageController m_messageController;
};
#endif  // MAINWINDOW_H
