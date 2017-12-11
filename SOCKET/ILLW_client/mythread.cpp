#include "mythread.h"
#include <QHostAddress>
#include <QTimer>
#include <QTimerEvent>

int k=0;
MyThread::MyThread(QObject *parent) : QObject(parent)
{

}

void MyThread::setFlag(bool flag)
{
    isStop = flag;
}

void MyThread::initSocket()
{

    //ipRec = "127.0.0.1";//测试用本机ip，实际工作时删掉这行

    if(isStop == false)
    {
        k=0;
        resendflag = false;
        file.setFileName(fileName);
        if (!file.open(QIODevice::ReadOnly))
        {
            return;
        }
        UdpSender = new QUdpSocket(this);
        UdpSender->bind(QHostAddress::Any, port);
        connect(UdpSender,&QUdpSocket::readyRead,this,&MyThread::readPendingDatagrams);//收到数据时调用
        timer=new QTimer(this);
        connect(timer,&QTimer::timeout,this,&MyThread::timerUpDate);
        sendData();//send第一个包
    }
}

void MyThread::sendData()
{

    if (!file.atEnd())
    {

        if(resendflag == false)//不需要重发
        {
            line = file.read(8000);//读取文件到line
        }

        QByteArray datagram;
        QDataStream out(&datagram,QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_5_4);
        out<<k<<line;//发送包的编号和包本身

        UdpSender->writeDatagram(datagram,QHostAddress(ipRec),portRec);

        //timer->start(100);
//        qDebug() <<"seq"<< k;
//        qDebug() <<"发送第"<< k << "包";


//        qDebug()<< k << line.size();

        emit mySignal(k);

        if(line.size() <8000)
        {
            emit finSignal();//发送结束信号
            file.close();
           // timer->stop();
        }
    }

    //发送完成开始计时
    timer->start(200);
    //m_nTimerID = this->startTimer(15);
}

void MyThread::readPendingDatagrams()
{
    //接收到了，停止计时
    timer->stop();
    //killTimer(m_nTimerID);



    resendflag = false;
    while (UdpSender->hasPendingDatagrams())
    {
        QHostAddress sender;
        quint16 senderPort;
        QByteArray datagram;
        datagram.resize(UdpSender->pendingDatagramSize());
        UdpSender->readDatagram(datagram.data(), datagram.size(), &sender, &senderPort);

        QDataStream in(&datagram, QIODevice::ReadOnly);
        in.setVersion(QDataStream::Qt_5_4);
        int ack;
        in>>ack;

//        qDebug() <<"接收到ack"<< ack<< "k"<< k;

        if(ack == k+1)//接收方的反馈，期待收到第k+1个包
        {
//            qDebug() <<"k加一";
            k++;//确认已经送达，发送下一个包
            sendData();
        }
        else
        {
            qDebug() <<"重传"<<"seq"<< k;
            resendflag = true;
            sendData();
        }

    }
    //计时器清零，开始计时
    //timer->setInterval(5000);

    //如果5秒还没有接收到则重传
}

void MyThread::timerUpDate ()
{

//    qDebug()<<"overtime";
    resendflag = true;
    if(UdpSender->bytesAvailable()!=0)//重启udp
    {
        qDebug()<<"重传seq"<< k;
        //qDebug()<<"接收不到数据，重启";
        UdpSender->close();
        UdpSender = new QUdpSocket(this);
        UdpSender->bind(QHostAddress::Any, port);
        connect(UdpSender,&QUdpSocket::readyRead,this,&MyThread::readPendingDatagrams);//收到数据时调用

        //resendflag = true;
        //sendData();
    }
    sendData();
}


/*void MyThread::timerEvent( QTimerEvent *event)
{
    if(event->timerId() == m_nTimerID)
    {
//    qDebug()<<"overtime";
    resendflag = true;

    if(UdpSender->bytesAvailable()!=0)//重启udp
    {
        qDebug()<<"重传seq"<< k;
        //qDebug()<<"接收不到数据，重启";
        UdpSender->close();
        UdpSender = new QUdpSocket(this);
        UdpSender->bind(QHostAddress::Any, port);
        connect(UdpSender,&QUdpSocket::readyRead,this,&MyThread::readPendingDatagrams);//收到数据时调用

        //resendflag = true;
        //sendData();
    }


    sendData();
    }
}*/

