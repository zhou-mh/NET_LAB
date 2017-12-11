#ifndef SIGNUPWIDGET_H
#define SIGNUPWIDGET_H

#include <QWidget>
#include <QTcpSocket>//通信套接字
#include <QMessageBox>

namespace Ui {
class SignupWidget;
}

class SignupWidget : public QWidget
{
    Q_OBJECT

public:
    explicit SignupWidget(QWidget *parent = 0);
    ~SignupWidget();

    QTcpSocket *s_Socket;//通信套接字
    QString s_ip;//服务器ip
    qint16 s_port;//服务器端口

private slots:
    void on_buttonSignup_clicked();

private:
    Ui::SignupWidget *ui;
    QString username;
};

#endif // SIGNUPWIDGET_H
