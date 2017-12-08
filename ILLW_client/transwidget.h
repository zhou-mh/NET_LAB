#ifndef TRANSWIDGET_H
#define TRANSWIDGET_H

#include <QWidget>
#include <QThread>
#include <QtNetwork>
#include <QTimer>
#include <QTime>
#include "mythread.h"

namespace Ui {
class TransWidget;
}

class TransWidget : public QWidget
{
    Q_OBJECT

public:
    explicit TransWidget(QWidget *parent = 0);
    ~TransWidget();
    void init(QString Server_ip,qint16 Server_port, QString username1);
    void sendmode(QString name);//进入发送模式
    void recvmode(QString name,QString filedata,QString files);//进入接收模式
    void getLocalIp();
    void dealClose();
    void timerUpDate();
    void timerUpDate2();

    QTime time;

    QTcpSocket *t_Socket;//通信套接字
    QString t_ip;//服务器ip
    qint16 t_port;//服务器端口
    QString username;//用户名
    QString sendername;//发送者名字
    QString recvname;//接收者名字
    QString localip;
    int port;//设定的端口

signals:
    //启动子线程的信号
    void startThread();//启动子线程的信号

private slots:
    void dealSignal(int sendcount);
    void dealfin();

    void on_buttonfile_clicked();

    void on_buttonconfirm_clicked();

    void on_buttontake_clicked();

    void on_buttonBind_clicked();

    void on_buttonrefuse_clicked();

    void readMessage();

    void sendbar();
    void recvbar();

    void initSocketRec();
    void readPendingDatagramsRec();

protected:
    void closeEvent(QCloseEvent *event);//关闭窗口事件

private:
    Ui::TransWidget *ui;

    QUdpSocket *UdpSender;
    QUdpSocket *UdpReader;
    QFile file;
    QString fileName;  //保存文件路径
    QString ipRec;//接收方ip
    int portRec;//接收方端口
    QString filee;
    qint64 fileSize;  //文件大小信息

    MyThread *myT;
    QThread *thread;



    QString localMessage; // 存放本地要发送的信息
    QString serverMessage;  //存放从服务器接收到的信息


    quint16 blockSize;  //存放接收到的信息大小
    QFile *localFile;  //要发送的文件
    qint64 totalBytes;  //数据总大小
    qint64 bytesWritten;  //已经发送数据大小
    qint64 bytesToWrite;   //剩余数据大小
    qint64 loadSize;   //每次发送数据的大小

    QByteArray outBlock;  //数据缓冲区，即存放每次要发送的数据

    //qint64 totalBytes;  //存放总大小信息
    qint64 bytesReceived;  //已收到数据的大小
    qint64 fileNameSize;  //文件名的大小信息
    //QString fileName;   //存放文件名
    //QFile *localFile;   //本地文件
    QByteArray inBlock;   //数据缓冲区

    QTimer *Timer1;
    QTimer *Timer2;

};

#endif // TRANSWIDGET_H
