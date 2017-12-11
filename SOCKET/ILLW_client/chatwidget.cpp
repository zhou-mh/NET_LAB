#include "chatwidget.h"
#include "ui_chatwidget.h"
#include <QHostAddress>
#include <QDebug>
#include <QMessageBox>

ChatWidget::ChatWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ChatWidget)
{
    ui->setupUi(this);
    this->setAutoFillBackground(true);
    QPalette palette;
    QPixmap pixmap("../ILLW_SOURCE/chatback.png");//设置背景
    palette.setBrush(QPalette::Window, QBrush(pixmap));
    this->setPalette(palette);

    setWindowTitle("ILLW");

    c_Socket = NULL;//初始化
    c_ip =("127.0.0.1");
    c_port = 8888;
    c_Socket = new QTcpSocket(this);//分配空间指定父对象

}

ChatWidget::~ChatWidget()
{
    delete ui;
}

void ChatWidget::init(QString Server_ip,qint16 Server_port, QString username1)
{
    c_ip = Server_ip;
    c_port = Server_port;
    username = username1;
    ui->username->setText(username);//设置名字
    c_Socket->connectToHost(QHostAddress(c_ip),c_port);//和服务器建立连接

    //组包并发送一条-getoffline指令
    QString getofflineflag="-getoffline";//获取离线信息标志
    QString data=getofflineflag+"#"+username;
    //用户名
    QByteArray message;
    QDataStream out(&message,QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_4);
    out<<data;
    c_Socket->write(message);//发送流

    connect(c_Socket,&QTcpSocket::readyRead,
            this,&ChatWidget::readMessage);

    tw.init(Server_ip, Server_port, username1);

}

void ChatWidget::readMessage()//读取信息并输出到窗口
{
    QDataStream in(c_Socket);
    in.setVersion(QDataStream::Qt_5_4);
    QString data;
    in >> data;

    QStringList list = data.split("#");//分割收到的字符串

    if(list[0]=="-login"&&list[2]==username)//不知道为什么收到的第一个返回信号是登陆。。
    {

        ui->pushButton->click();//总之再按一次按键就可以了
    }
    if(list[0]=="-checkchat"&&list[2]==username)
    {
        if(list[1]=="true")
            ui->status->setText("用户在线");
        else
            ui->status->setText("用户离线或不存在");
    }
    if(list[0]=="-send"&&list[2]==username)//发送给本用户名的在线信息
    {
        QString temp = QString("[%1说:%2]").arg(list[1]).arg(list[3]);//组包输出
        ui->textEditRead->append(temp);//多行显示
    }
    if(list[0]=="-getoffline"&&list[1]==username)//发送给本用户名的离线信息
    {
        QString temp = QString("离线信息[%1说:%2]").arg(list[2]).arg(list[3]);//组包输出
        ui->textEditRead->append(temp);//多行显示
    }
    if(list[0]=="-file"&&list[2]==username)//发送文件确认
        {
            tw.recvmode(list[1],list[3],list[4]);
            tw.show();
            qDebug()<<"shoudaola!";
        }

}


void ChatWidget::closeEvent(QCloseEvent *event)
{
    //发送离线信号
    //组包并发送
    QString logoutflag="-logout";//离线标志
    QString data=logoutflag+"#"+username;
    //用户名

    QByteArray message;
    QDataStream out(&message,QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_4);
    out<<data;
    c_Socket->write(message);//发送流
    c_Socket->disconnectFromHost();

}

void ChatWidget::on_pushButton_clicked()//聊天对象确认
{
    ui->username->setText(username);
    //获取编辑区内容
    friendname = ui->lineEdit->text();
    ui->labelfriendname->setText(friendname);
    //qDebug()<<friendname;
    if(friendname=="")
    {
        QMessageBox::information(this,"警告","输入不能为空",QMessageBox::Ok);
        return;
    }

    //组包并发送
    QString checkflag="-checkchat";//聊天确认标识
    QString data=checkflag+"#"+username+"#"+friendname;
    QByteArray message;
    QDataStream out(&message,QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_4);
    out<<data;
    c_Socket->write(message);//发送流

}

void ChatWidget::on_buttonSend_clicked()//发送信息
{
    if(friendname=="")
    {
        QMessageBox::information(this,"警告","未指定发送对象",QMessageBox::Ok);
        return;
    }
    //获取编辑区内容
    QString str = ui->textEditWrite->toPlainText();
    if(str=="")
    {
        QMessageBox::information(this,"警告","输入不能为空",QMessageBox::Ok);
        return;
    }

    //组包并发送
    QString sendflag="-send";//聊天确认标识
    QString data=sendflag+"#"+username+"#"+friendname+"#"+str;
    QByteArray message;
    QDataStream out(&message,QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_4);
    out<<data;
    c_Socket->write(message);//发送流


    QString temp = QString("[我对%1说:%2]").arg(friendname).arg(str);//组包输出
    ui->textEditRead->append(temp);//多行显示
    ui->textEditWrite->clear();
}

void ChatWidget::on_buttonFile_clicked()
{
    if(friendname=="")
       {
           QMessageBox::information(this,"警告","未指定发送对象",QMessageBox::Ok);
           return;
       }

    tw.sendmode(friendname);
    tw.show();
}
