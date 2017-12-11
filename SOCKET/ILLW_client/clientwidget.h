#ifndef CLIENTWIDGET_H
#define CLIENTWIDGET_H

#include <QWidget>
#include <QTcpSocket>//通信套接字
#include <QMessageBox>
#include "signupwidget.h"
#include "findback.h"
#include "chatwidget.h"


namespace Ui {
class ClientWidget;
}

class ClientWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ClientWidget(QWidget *parent = 0);
    ~ClientWidget();

signals:


private slots:
    void on_buttonLogin_clicked();

    void on_buttonSignup_clicked();

    void on_buttonFindback_clicked();

    void on_buttonSettings_clicked();

    void on_buttonSave_clicked();

protected:
    void closeEvent(QCloseEvent *event);//关闭窗口事件
private:
    Ui::ClientWidget *ui;
    QTcpSocket *tcpSocket;//通信套接字
    QString Server_ip;//服务器ip
    qint16 Server_port;//服务器端口

    SignupWidget sw;//注册子窗口
    Findback fw;
    ChatWidget cw;//聊天窗口
    QString username;

};

#endif // CLIENTWIDGET_H
