#ifndef FINDBACK_H
#define FINDBACK_H

#include <QWidget>
#include <QTcpSocket>//通信套接字
#include <QMessageBox>

namespace Ui {
class Findback;
}

class Findback : public QWidget
{
    Q_OBJECT

public:
    explicit Findback(QWidget *parent = 0);
    ~Findback();

    QTcpSocket *f_Socket;//通信套接字
    QString f_ip;//服务器ip
    qint16 f_port;//服务器端口

private slots:
     void on_pushButton_clicked();

     void on_pushButton_2_clicked();

private:
    Ui::Findback *ui;
    QString username;
};

#endif // FINDBACK_H
