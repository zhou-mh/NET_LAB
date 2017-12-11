#ifndef CHATWIDGET_H
#define CHATWIDGET_H

#include <QWidget>
#include <QTcpSocket>
#include "transwidget.h"

namespace Ui {
class ChatWidget;
}

class ChatWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ChatWidget(QWidget *parent = 0);
    ~ChatWidget();

    void init(QString Server_ip,qint16 Server_port, QString username);

    QTcpSocket *c_Socket;//通信套接字
    QString c_ip;//服务器ip
    qint16 c_port;//服务器端口
    QString username;
    QString friendname;



signals:

protected:
    void closeEvent(QCloseEvent *event);//关闭窗口事件
private slots:
    void on_pushButton_clicked();

    void on_buttonSend_clicked();

    void readMessage();

    void on_buttonFile_clicked();

private:
    Ui::ChatWidget *ui;

    TransWidget tw;

};

#endif // CHATWIDGET_H
