#include "findback.h"
#include "ui_findback.h"
#include <QHostAddress>
#include <QMessageBox>

Findback::Findback(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Findback)
{
    ui->setupUi(this);

    setWindowTitle("找回密码");
    f_Socket = NULL;//初始化
    f_ip =("127.0.0.1");
    f_port = 8888;
    f_Socket = new QTcpSocket(this);//分配空间指定父对象
}

Findback::~Findback()
{
    delete ui;
}

void Findback::on_pushButton_clicked()//找回密码
{
    //获取编辑区内容
    QString Question = ui->lineEditQuestion->text();
    if(Question=="")
    {
        QMessageBox::information(this,"警告","问题不能为空",QMessageBox::Ok);
        return;
    }
    username = ui->lineEditID->text();
    QString Answer = ui->lineEditAnswer->text();
    QString Newpw = ui->lineEditNewpw->text();
    if(username=="" || Newpw==""||Answer=="")
    {
        QMessageBox::information(this,"警告","输入不能为空",QMessageBox::Ok);
        return;
    }
    //组包
    QString findbackflag="-findback";//找回密码标志
    QString data=findbackflag+"#"+username+"#"+Answer+"#"+Newpw;
    //发送数据
    QByteArray message;
    QDataStream out(&message,QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_4);
    out<<data;
    f_Socket->write(message);//发送流
 /*   connect(f_Socket,&QTcpSocket::readyRead,
            [=]()
    {
        QDataStream in(f_Socket);
        in.setVersion(QDataStream::Qt_5_4);
        QString data;
        in >> data;

        QStringList list = data.split("#");//分割收到的字符串
        if(list[0]=="-findback"&&list[2]==username)//第一个命令为找回密码
        {
            if(list[1]=="true")
                QMessageBox::information(this,"提示","找回密码成功",QMessageBox::Ok);
            else
                QMessageBox::information(this,"提示","找回密码失败",QMessageBox::Ok);
        }
    }
            );*/
    //f_Socket->disconnectFromHost();
}

void Findback::on_pushButton_2_clicked()//获取
{
    //主动和服务器建立连接
    f_Socket->connectToHost(QHostAddress(f_ip),f_port);
    //获取编辑区内容
    username = ui->lineEditID->text();
    if(username=="")
    {
        QMessageBox::information(this,"警告","输入不能为空",QMessageBox::Ok);
        return;
    }
    //组包
    QString getbackflag="-getback";//获取密保问题标志
    QString data=getbackflag+"#"+username;
    //发送数据
    QByteArray message;
    QDataStream out(&message,QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_4);
    out<<data;
    f_Socket->write(message);//发送流

    connect(f_Socket,&QTcpSocket::readyRead,
            [=]()
    {
        QDataStream in(f_Socket);
        in.setVersion(QDataStream::Qt_5_4);
        QString data;
        in >> data;

        QStringList list = data.split("#");//分割收到的字符串

        if(list[0]=="-getback"&&list[2]==username)//第一个命令为获取密保问题
        {
            if(list[1]=="")
                QMessageBox::information(this,"提示","不存在该用户",QMessageBox::Ok);
            else
                {
                    ui->lineEditQuestion->setText(QString(list[1]));
                    //LineEdit->setText(QString("你要赋值的内容！"));
                }
        }

        if(list[0]=="-findback"&&list[2]==username)//第一个命令为找回密码
        {
            if(list[1]=="true")
                QMessageBox::information(this,"提示","找回密码成功",QMessageBox::Ok);
            else
                QMessageBox::information(this,"提示","找回密码失败",QMessageBox::Ok);
        }
    }
            );
    //f_Socket->disconnectFromHost();

}
