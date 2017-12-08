#include "signupwidget.h"
#include "ui_signupwidget.h"
#include <QDebug>
#include <QHostAddress>
#include <QMessageBox>

SignupWidget::SignupWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SignupWidget)
{
    ui->setupUi(this);

    setWindowTitle("账号注册");
    s_Socket = NULL;//初始化
    s_ip =("127.0.0.1");
    s_port = 8888;
    s_Socket = new QTcpSocket(this);//分配空间指定父对象

}

SignupWidget::~SignupWidget()
{

    delete ui;
}

void SignupWidget::on_buttonSignup_clicked()
{
    //主动和服务器建立连接
    s_Socket->connectToHost(QHostAddress(s_ip),s_port);
    //获取编辑区内容
    username = ui->lineEditID->text();
    QString passward=ui->lineEditPassword->text();
    QString CP = ui->lineEditCP->text();
    QString Question = ui->lineEditQuestion->text();
    QString Answer = ui->lineEditAnswer->text();
    if(username=="" || passward==""||CP==""||Question==""||Answer=="")
    {
        QMessageBox::information(this,"警告","输入不能为空",QMessageBox::Ok);
        return;
    }
    if(passward!=CP)
    {
        QMessageBox::information(this,"警告","两次密码输入不一致",QMessageBox::Ok);
        return;
    }
    //组包
    QString signupflag="-signup";//注册标志
    QString data=signupflag+"#"+username+"#"+passward+"#"+Question+"#"+Answer;
    //发送注册数据
    QByteArray message;
    QDataStream out(&message,QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_4);
    out<<data;
    s_Socket->write(message);//发送流
    //s_Socket->write(data.toLatin1());

    connect(s_Socket,&QTcpSocket::readyRead,
            [=]()
    {
        QDataStream in(s_Socket);
        in.setVersion(QDataStream::Qt_5_4);
        QString data1;
        in >> data1;

        QStringList list = data1.split("#");//分割收到的字符串
        if(list[0]=="-signup"&&list[2]==username)//第一个命令为注册
        {
            if(list[1]=="true")
                QMessageBox::information(this,"提示","注册成功",QMessageBox::Ok);
            else
                QMessageBox::information(this,"提示","注册失败",QMessageBox::Ok);
        }
    }
            );
    //s_Socket->disconnectFromHost();
}
