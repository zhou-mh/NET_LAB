#ifndef SERVERWIDGET_H
#define SERVERWIDGET_H

#include <QWidget>
#include <QTcpServer>//监听套接字
#include <QTcpSocket>//通信套接字
#include <QMessageBox>
#include <QList>//同时与多个客户端连接的socket放在list容器中
#include <QTimer>
#include "sqlite.h"

namespace Ui {
class ServerWidget;
}

class ServerWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ServerWidget(QWidget *parent = 0);
    ~ServerWidget();

    bool checkLogin(QString username,QString password);
    bool checkSignUp(QString username,QString password,QString Question,QString Answer);
    bool checkFindback(QString username,QString Answer,QString Newpw);
    QString checkGetback(QString username);
    void checkLogout(QString username);
    bool checkChat(QString friendname);

private slots:
    void on_buttonstart_clicked();

    void on_buttondisconnect_clicked();

    void on_pushButton_clicked();

    void newConnect();

    void readMessage();

    void sendMessage(QString infomation);

private:
    Ui::ServerWidget *ui;
    QTcpServer *tcpServer;//监听套接字
    QTcpSocket *tcpSocket;//通信套接字

    QList<QTcpSocket*> *socket_list;
    QString message;

    Sqlite sw;//数据库
    QTimer timer;//定时器
};

#endif // SERVERWIDGET_H
