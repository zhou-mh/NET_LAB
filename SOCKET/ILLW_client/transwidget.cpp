#include "transwidget.h"
#include "ui_transwidget.h"
#include <QFileDialog>
#include <QFileInfo>
#include <QDebug>
#include <QHostAddress>
#include <QMessageBox>
#include <QNetworkInterface>
#include <QThread>
#include <QTime>

int j = 0,j1=0;
int i = 0,i1=0;        //receive
TransWidget::TransWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TransWidget)
{
    ui->setupUi(this);

    ui->buttonconfirm->setEnabled(false);
    ui->buttonfile->setEnabled(true);
    ui->buttonrefuse->setEnabled(true);
    ui->buttontake->setEnabled(true);
    ui->widgetRecv->setVisible(false);//初始隐藏接受按钮，在接收到信号时显示
    t_Socket = NULL;//初始化
    t_ip =("127.0.0.1");//服务器ip
    t_port = 8888;//服务器端口
    t_Socket = new QTcpSocket(this);//分配空间指定父对象

    port=5555;//初始为5555
    UdpReader = NULL;
    //UdpSender = NULL;




    ui->progressBar->setValue(0);

    //动态分配空间，不能指定父对象
    myT = new MyThread();

    //创建子线程
    thread = new QThread(this);

    //把自定义的线程加入到子线程
    myT->moveToThread(thread);
    connect(this,&TransWidget::destroyed,this,&TransWidget::dealClose);
    connect(this,&TransWidget::startThread,myT,&MyThread::initSocket);
    connect(myT,&MyThread::mySignal,this,&TransWidget::dealSignal);
    connect(myT,&MyThread::finSignal,this,&TransWidget::dealfin);

    Timer2 = NULL;
    Timer2 =new QTimer(this);
    connect(Timer2,&QTimer::timeout,this,&TransWidget::timerUpDate2);
    Timer2->start(1000);

}

TransWidget::~TransWidget()
{
    delete ui;
}

void TransWidget::dealClose()
{
    if(thread->isRunning() == false)
    {
        return;
    }
    myT->setFlag(true);
    thread->quit();
    thread->wait();
    delete myT;
}

void TransWidget::init(QString Server_ip,qint16 Server_port, QString username1)
{

    t_ip = Server_ip;
    t_port = Server_port;
    username = username1;
    t_Socket->connectToHost(QHostAddress(t_ip),t_port);//和服务器建立连接，便于接受
    connect(t_Socket,&QTcpSocket::readyRead,
            this,&TransWidget::readMessage);
}

void TransWidget::readMessage()
{
    QDataStream in(t_Socket);
    in.setVersion(QDataStream::Qt_5_4);
    QString data;
    in >> data;

    QStringList list = data.split("#");//分割收到的字符串
    if(list[0]=="-tofile"&&list[1]=="true"&&list[2]==username)//对方已接受
    {
        QString str = QString("用户[%1]正在接收文件").arg(list[3]);
        ui->textEditmessage->append(str);
        ui->labelstate->setText("正在发送");
        //list[4]是接收方ip，list[5]是接收方端口
        //发送文件

        getLocalIp();
        ipRec = list[4];
        portRec = QString(list[5]).toInt();


        if(thread->isRunning()==true)
        {
            return;
        }
        //启动线程，但是没有启动线程处理函数
        thread->start();
        myT->setFlag(false);

        myT->port = port;
        myT->ipRec = ipRec;
        myT->portRec = portRec;
        myT->fileName = fileName;//这里可能需要传文件名，在线程里打开
        //通过signal-slot方式调用
        emit startThread();
        time.start();





    }
    if(list[0]=="-tofile"&&list[1]=="false"&&list[2]==username)//对方拒绝接受
    {
        QString str = QString("用户[%1]拒绝接收文件").arg(list[3]);
        ui->textEditmessage->append(str);
        ui->labelstate->setText("对方拒绝接受");
    }
}

void TransWidget::closeEvent(QCloseEvent *event)//关闭窗口事件
{
    if(thread->isRunning() == false)//关闭线程
    {
        return;
    }
    myT->setFlag(true);
    thread->quit();
    thread->wait();
    delete myT;
    t_Socket->disconnectFromHost();
}

void TransWidget::on_buttonfile_clicked()//选择文件，获取文件名
{

    fileName = QFileDialog::getOpenFileName(this);
    if(!fileName.isEmpty())
    {
        QString currentFileName = fileName.right(fileName.size() - fileName.lastIndexOf('/')-1);
        QString str = QString("文件名：%1").arg(currentFileName);
        ui->textEditmessage->append(str);


        file.setFileName(fileName);
        if (!file.open(QIODevice::ReadOnly))
        {
            ui->labelstate->setText("请重新选择文件");
            return;
        }

        fileSize= file.size();

        file.close();

        ui->labelstate->setText("打开文件成功！");
        ui->buttonconfirm->setEnabled(true);
    }
    else
    {
        ui->labelstate->setText("请重新选择文件");
    }
}

void TransWidget::sendmode(QString name)//进入发送者模式
{
    recvname = name;
    QString str = QString("向用户[%1]发送文件").arg(recvname);
    ui->textEditmessage->append(str);
    ui->widgetSend->setVisible(true);
    ui->widgetRecv->setVisible(false);
}

void TransWidget::recvmode(QString name,QString filedata,QString files)//接收者模式
{
    sendername = name;//发送者的名字

    fileSize = files.toInt();//文件大小

    filee = filedata;//文件名


    QString str = QString("收到用户[%1]发送的文件%2，大小为%3KB").arg(sendername).arg(filedata).arg(fileSize/1024);
    ui->textEditmessage->append(str);
    ui->widgetSend->setVisible(false);
    ui->widgetRecv->setVisible(true);
}

void TransWidget::on_buttonconfirm_clicked()//发送
{
    if(port==0)//还没绑定端口
    {
        QMessageBox::about(this,"提示","请先绑定端口");
        return;
    }
    //组包并发送
    QString currentFileName = fileName.right(fileName.size() - fileName.lastIndexOf('/')-1);
    QString fileflag="-file";//发送文件标识
    QString files = QString("%1").arg(fileSize);//发送文件大小
    QString data=fileflag+"#"+username+"#"+recvname+"#"+currentFileName+"#"+files;
    QByteArray message;
    QDataStream out(&message,QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_4);
    out<<data;
    t_Socket->write(message);//发送流
    ui->labelstate->setText("已发送，等待对方接受！");
}

void TransWidget::on_buttontake_clicked()//确认接收
{
    if(port==0)//还没绑定端口
    {
        QMessageBox::about(this,"提示","请先绑定端口");
        return;
    }
    getLocalIp();
    //组包并发送
    QString fileflag="-tofile";//发送文件标识
    QString p = QString("%1").arg(port);
    QString data=fileflag+"#true#"+sendername+"#"+username+"#"+localip+"#"+p;
    QByteArray message;
    QDataStream out(&message,QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_4);
    out<<data;
    t_Socket->write(message);//发送流
    ui->labelstate->setText("正在接收");

    //开始接受udp信息
    //filee为接受的文件名
    initSocketRec();
    time.start();



}

void TransWidget::on_buttonBind_clicked()//绑定端口
{
    bool ok;
    port = ui->lineEditPort->text().toInt(&ok);
    if(ok == false)
    {
        QMessageBox::about(this,"警告","端口号输入有误");
        port = 5555;//把端口赋值为5555
    }
    else
    {
        QMessageBox::about(this,"提示","端口绑定成功");
    }

}

void TransWidget::on_buttonrefuse_clicked()//拒绝接受
{
    //组包并发送
    QString fileflag="-tofile";//发送文件标识
    QString data=fileflag+"#false#"+sendername+"#"+username+"#0.0.0.0#0";
    QByteArray message;
    QDataStream out(&message,QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_4);
    out<<data;
    t_Socket->write(message);//发送流
    ui->labelstate->setText("已拒绝接收");
}

void TransWidget::getLocalIp()//获取本地ip，赋值给localip
{
    /*
    QString localHostName = QHostInfo::localHostName();

    qDebug() <<"localHostName:"<<localHostName;
    QHostInfo info = QHostInfo::fromName(localHostName);
    foreach(QHostAddress address,info.addresses())
    {
        if(address.protocol() == QAbstractSocket::IPv4Protocol)
            localip= address.toString();
    }
    */


   QList<QHostAddress> list = QNetworkInterface::allAddresses();
    foreach(QHostAddress add ,list)
    {
        if(add.protocol() == QAbstractSocket::IPv4Protocol)
        {
            //if(add.toString().contains("192.168.")||add.toString().contains("127.0."))//机房改成这样的
            if(add.toString().contains("127.0."))//自己电脑用这个
            {
                continue;
            }
            localip = add.toString();
        }
    }


  //  localip= "127.0.0.1";//测试用本机ip
}


void TransWidget::initSocketRec()
{
    qDebug() << "recving"<<port;
    UdpReader = new QUdpSocket(this);
    UdpReader->bind(QHostAddress::Any, port);
    connect(UdpReader,&QUdpSocket::readyRead,this,&TransWidget::readPendingDatagramsRec);

    file.setFileName( filee );

    qDebug() << filee;

    if (!file.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Unbuffered))
        return;
    filee.clear();

    Timer1=new QTimer(this);
    connect(Timer1,&QTimer::timeout,this,&TransWidget::timerUpDate);
}

void TransWidget::readPendingDatagramsRec()
{
    Timer1->stop();
//    qDebug() <<"recving";
    while (UdpReader->hasPendingDatagrams())
    {
        QHostAddress sender;
        quint16 senderPort;
        QByteArray datagram;
        QByteArray data;
        datagram.resize(UdpReader->pendingDatagramSize());
        UdpReader->readDatagram(datagram.data(), datagram.size(), &sender, &senderPort);
        QDataStream in(&datagram, QIODevice::ReadOnly);
        in.setVersion(QDataStream::Qt_5_4);
        int seq;
        in>>seq>>data;//分割接收到的数据流

//        qDebug() <<"seq"<<seq<<"i"<<i;

        if(seq==i)
        {
//            qDebug() <<"接收第"<< i << "包";
            file.write(data.data(),data.size());//写入接收到的文件
            i++;

            //发送ack[i]
//            qDebug() <<"ack"<< i ;

            QByteArray datagrami;
            QDataStream outi(&datagrami,QIODevice::WriteOnly);
            outi.setVersion(QDataStream::Qt_5_4);
            outi<<i;//ack的编号
            UdpReader->writeDatagram(datagrami,sender ,senderPort);


            //UdpReader->writeDatagram(datagrami,sender ,senderPort);//第二遍
        }


        else //if(seq+1==i)//ack丢包了，发送方还是发上一个包过来
        {
            //发送ack[i]，让他发需要的下一个包
            qDebug() <<"重发ack"<< i ;

            QByteArray datagramii;
            QDataStream outii(&datagramii,QIODevice::WriteOnly);
            outii.setVersion(QDataStream::Qt_5_4);
            outii<<i;//ack的编号
            UdpReader->writeDatagram(datagramii,sender ,senderPort);

            //UdpReader->writeDatagram(datagramii,sender ,senderPort);
        }

        Timer1->setInterval(200);
        Timer1->start();

//        qDebug() << i << datagram.size();

        recvbar();//更新数据


        if(datagram.size() <8000)
        {
            Timer2->stop();
            ui->progressBar->setValue(fileSize);//进度条变满
            QString str = QString("接收完成，速率为%1KB/S").arg(fileSize/time.elapsed());
            ui->labelstate->setText(str);
            Timer1->stop();
        }
    }
}

void TransWidget::sendbar()
{
    //更新进度条
    ui->progressBar->setMaximum(fileSize);
    ui->progressBar->setValue(8000*j);
}

void TransWidget::recvbar()
{
    //更新进度条，这里可以计算实时速度
    ui->progressBar->setMaximum(fileSize);
    ui->progressBar->setValue(8000*i);
}

void TransWidget::dealSignal(int sendcount)//处理线程信号
{
    //sendcount已经发送的包，*8000=已经发送的大小
    j=sendcount;
    sendbar();
}

void TransWidget::dealfin()//处理线程信号
{
    Timer2->stop();
    QString str = QString("发送完成，速率为%1KB/S").arg(fileSize/time.elapsed());
    ui->labelstate->setText(str);
    ui->progressBar->setValue(fileSize);//进度条变满
    if(thread->isRunning() == false)
    {
        return;
    }
    myT->setFlag(true);
    thread->quit();
    thread->wait();
}

void TransWidget::timerUpDate()
{
    if(UdpReader->bytesAvailable()!=0)//重启udp
    {
//        qDebug()<<"接收不到数据，重启";
        UdpReader->close();
        UdpReader = new QUdpSocket(this);
        UdpReader->bind(QHostAddress::Any, port);
        connect(UdpReader,&QUdpSocket::readyRead,this,&TransWidget::readPendingDatagramsRec);
    }
}

void TransWidget::timerUpDate2()
{
    if(i!=0)//在接收
    {
        QString str = QString("%1KB/S").arg((i-i1)*8);
        ui->labelspeed->setText(str);
        i1=i;
    }
    if(j!=0)//在发送
    {
        QString str = QString("%1KB/S").arg((j-j1)*8);
        ui->labelspeed->setText(str);
        j1=j;
    }
    Timer2->start(1000);
}
