#include "serverwidget.h"
#include "ui_serverwidget.h"
#include <QDebug>
#include <QStringList>
#include <QTimer>
#include <QTime>

ServerWidget::ServerWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ServerWidget)
{
    ui->setupUi(this);
    setWindowTitle("ILLW服务器");
    ui->buttonstart->setEnabled(false);//进入时自动打开服务器
    ui->buttondisconnect->setEnabled(true);
    tcpServer = NULL;
    tcpSocket = NULL;
    tcpServer = new QTcpServer(this);//指定父对象，自动回收空间
    tcpSocket = new QTcpSocket(this);
    socket_list = new QList<QTcpSocket*>;
    //BIND()LISTEN()监听，绑定8888端口
    tcpServer->listen(QHostAddress::Any,8888);

    connect(tcpServer,&QTcpServer::newConnection,
            this,&ServerWidget::newConnect);

}

ServerWidget::~ServerWidget()
{
    delete ui;
}

void ServerWidget::newConnect()
{
    //ACCEPT()，取出建立好连接的套接字
    tcpSocket = tcpServer->nextPendingConnection();
    //获取客户的ip和端口
    QString ip = tcpSocket->peerAddress().toString();//地址转换成字符串
    qint16 port = tcpSocket->peerPort();//端口
    QString temp = QString("[%1:%2成功连接]").arg(ip).arg(port);//把地址和端口组包输出
    ui->textEditRead->append(temp);

    socket_list->append(tcpSocket);//有一个连接成功就取出这个套接字，放在list中

    connect(tcpSocket,&QTcpSocket::readyRead,
            this,&ServerWidget::readMessage);

}

void ServerWidget::readMessage()
{
    for(int i = 0;i < socket_list->length();i++)//读取所有套接字发送的信息
    {
        QDataStream in(socket_list->at(i));
        in.setVersion (QDataStream::Qt_5_4);
        in >> message;
        if(!(message.isEmpty()))
        {
            break;
        }
    }
    ui->textEditRead->append(tr("%1").arg(message));//显示收到的信息字符串
    QStringList list = message.split("#");//分割收到的字符串
    bool ret=0;
    bool ter=0;
    QString str;
    QString rts;
    if(list[0]=="-login")//第一个命令为登录
        ret=checkLogin(list[1],list[2]);//用户名#密码
    else if(list[0]=="-signup")//第一个命令为注册
        ret=checkSignUp(list[1],list[2],list[3],list[4]);//用户名#密码#密保问题#密保答案
    else if(list[0]=="-findback")//第一个命令为找回密码
        ret=checkFindback(list[1],list[2],list[3]);//用户名#密保答案#新密码
    else if(list[0]=="-getback")//第一个命令为获取密保问题
        str = checkGetback(list[1]);//用户名
    else if(list[0]=="-logout")//第一个命令为离线
        checkLogout(list[1]);//用户名
    else if(list[0]=="-checkchat")//第一个命令为聊天确认
        ret=checkChat(list[2]);//好友名

    QString sendData=list[0];//对应操作命令

    if(list[0]=="-file")//文件命令
    {
        sendData+="#";
        sendData+=list[1];//发送方
        sendData+="#";
        sendData+=list[2];//接收方
        sendData+="#";
        sendData+=list[3];//文件名
        sendData+="#";
        sendData+=list[4];//文件大小
    }
    if(list[0]=="-tofile")//文件命令
    {
        sendData+="#";
        sendData+=list[1];//是否拒绝
        sendData+="#";
        sendData+=list[2];//发送方
        sendData+="#";
        sendData+=list[3];//接收方
        sendData+="#";
        sendData+=list[4];//接受方ip
        sendData+="#";
        sendData+=list[5];//接收方端口
    }
    if(list[0]=="-getoffline")//获取离线信息命令
    {

        for(int i =0;i<sw.modeloff->rowCount();i++)//遍历逐行找对应的离线信息发送回去
        {
           sendData=list[0];
           rts = sw.findoff(i,list[1]);//参数为行号和接收方，返回的rts=发送方#信息
           if(rts!="")
           {
               //发送离线信息
               sendData+="#";
               sendData+=list[1];//获取离线信息的用户，即接收方
               sendData+="#";
               sendData+=rts;
               sendMessage (sendData);//发送完离线信息后
               sw.modeloff->removeRow(i);//把本地的离线消息从数据库中删除
           }
           else
               continue;
           //设置发送间隔,防止tcp黏包
           QTime t;
           t.start();
           while(t.elapsed()<20)
               QCoreApplication::processEvents();
        }
        return;
    }
    if(list[0]=="-send")//第一个命令为发送信息
    {
        ter=checkChat(list[2]);//确认接收方是否在线
        if(ter==true)//在线
        {
            sendData+="#";
            sendData+=list[1];//发送方
            sendData+="#";
            sendData+=list[2];//接收方
            sendData+="#";
            sendData+=list[3];//信息
            sendMessage (sendData);//发送操作结果
            return;
        }
        else//对方离线，把离线消息写入数据库
        {
           sw.newoff(list[1],list[2],list[3]);
           return;
        }
    }
    if(list[0]=="-logout"||list[0]=="-send")
        return;
    if(list[0]=="-getback")
    {
        sendData+="#";
        sendData+=str;
        sendData+="#";
        sendData+=list[1];
    }
    else
    {
        if(ret)
        {
            sendData+="#true#";
            sendData+=list[1];
        }
        else
        {
            sendData+="#false#";
            sendData+=list[1];
        }
    }
    sendMessage (sendData);//发送操作结果
}


void ServerWidget::sendMessage(QString sendData)
{
    QByteArray message;
    QDataStream out(&message,QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_4);
    out<<sendData;

    //qDebug()<<sendData;
    for (int i = 0;i < socket_list->length();i ++)//所有的客户端都会收到这条信息,需要客户端自己筛选是不是发送给自己的
    {
        socket_list->at(i)->write(message);
    }
}


void ServerWidget::on_buttonstart_clicked()//按下开启服务器
{
    ui->buttonstart->setEnabled(false);//按钮变灰
    ui->buttondisconnect->setEnabled(true);
    newConnect();//建立连接
}

void ServerWidget::on_buttondisconnect_clicked()//按下断开服务器按钮
{
    ui->buttonstart->setEnabled(true);
    ui->buttondisconnect->setEnabled(false);
    if(tcpSocket == NULL)
    {
        return;
    }
    //主动和客户端断开连接
    tcpSocket->disconnectFromHost();
    tcpSocket->close();
    tcpSocket = NULL;

}

bool ServerWidget::checkLogin(QString username,QString password)
{
    bool ret = sw.login(username,password);
    return ret;
}

bool ServerWidget::checkSignUp(QString username,QString password,QString Question,QString Answer)
{
    bool ret = sw.signup(username,password,Question,Answer);
    return ret;
}

bool ServerWidget::checkFindback(QString username,QString Answer,QString Newpw)
{
    bool ret = sw.findback(username,Answer,Newpw);
    return ret;
}

QString ServerWidget::checkGetback(QString username)
{
    QString str;
    str = sw.getback(username);
    return str;
}

void ServerWidget::checkLogout(QString username)
{
    sw.logout(username);
}

bool ServerWidget::checkChat(QString friendname)
{
    bool ret = sw.checkchat(friendname);
    return ret;
}

void ServerWidget::on_pushButton_clicked()
{
    //显示子窗口
    sw.show();
}
