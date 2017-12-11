#ifndef MYTHREAD_H
#define MYTHREAD_H

#include <QObject>
#include <QtNetwork>
#include <QTimer>

class MyThread : public QObject
{
    Q_OBJECT
public:
    explicit MyThread(QObject *parent = 0);

    //线程处理函数
    void initSocket();

    void setFlag(bool flag= true);

    bool isStop;
    bool resendflag;

    void sendData();
    void readPendingDatagrams();

    void timerUpDate();

    QUdpSocket *UdpSender;
    QFile file;
    QString fileName;  //保存文件路径
    QString ipRec;//接收方ip
    int portRec;//接收方端口
    QString filee;
    qint64 fileSize;  //文件大小信息
    QString localip;
    int port;//设定的端口
    QByteArray lastline;
    QByteArray line;
    QByteArray lastdatagram;
    QTimer *timer;
   // QTimer *timer1;

    //virtual void timerEvent( QTimerEvent *event);
    int m_nTimerID;

signals:
    void mySignal(int sendcount);
    void finSignal();

public slots:

};

#endif // MYTHREAD_H
