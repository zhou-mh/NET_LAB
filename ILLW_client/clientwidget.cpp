#include "clientwidget.h"
#include "ui_clientwidget.h"
#include <QHostAddress>
#include <QDebug>
#include <QMessageBox>
#include <signupwidget.h>
#include <QCloseEvent>

ClientWidget::ClientWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ClientWidget)
{
    ui->setupUi(this);
    ui->Settings->setVisible(false);
    setWindowTitle("ILLW客户端");
    //sw=NULL;
    tcpSocket = NULL;//初始化
    Server_ip = ("127.0.0.1");
    Server_port = 8888;
    tcpSocket = new QTcpSocket(this);//分配空间指定父对象


    //SignupWidget::SignupWidget(this,Server_ip,Server_port);
}

ClientWidget::~ClientWidget()
{
    delete ui;
}

void ClientWidget::on_buttonLogin_clicked()
{
    //主动和服务器建立连接
    tcpSocket->connectToHost(QHostAddress(Server_ip),Server_port);
    //获取编辑区内容
    username = ui->lineEditID->text();
    QString passward=ui->lineEditPassword->text();
    if(username=="" || passward=="")
    {
        QMessageBox::information(this,"警告","输入不能为空",QMessageBox::Ok);
        return;
    }
    //组包并发送
    QString loginflag="-login";//登录标志
    QString data=loginflag+"#"+username+"#"+passward;
    //用户名和密码

    QByteArray message;
    QDataStream out(&message,QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_4);
    out<<data;
    tcpSocket->write(message);//发送流
    //tcpSocket->write(data.toLatin1());

    connect(tcpSocket,&QTcpSocket::readyRead,
            [=]()
    {
        QDataStream in(tcpSocket);
        in.setVersion(QDataStream::Qt_5_4);
        QString data;
        in >> data;

        QStringList list = data.split("#");//分割收到的字符串
        if(list[0]=="-login"&&list[2]==username)//第一个命令为登录
        {
            if(list[1]=="true")
            {
                QMessageBox::information(this,"提示","登录成功",QMessageBox::Ok);//登录成功,关闭其他窗口
                cw.init(Server_ip,Server_port,username);//初始化
                cw.show();
                sw.s_Socket->disconnectFromHost();
                fw.f_Socket->disconnectFromHost();
                tcpSocket->disconnectFromHost();
                sw.close();
                fw.close();
                this->close();//关闭其他窗口

            }
            else
                QMessageBox::information(this,"提示","登录失败",QMessageBox::Ok);//登录失败
        }
    }
            );

}

void ClientWidget::on_buttonSignup_clicked()
{
    //显示注册子窗口
    sw.s_ip = Server_ip;
    sw.s_port = Server_port;
    sw.show();
}

void ClientWidget::on_buttonFindback_clicked()
{
    //显示找回密码子窗口
    fw.f_ip = Server_ip;
    fw.f_port = Server_port;
    fw.show();
}

void ClientWidget::on_buttonSettings_clicked()
{
    //显示网络设置
    ui->Settings->setVisible(true);
}

void ClientWidget::on_buttonSave_clicked()
{
    //保存设置并关闭网络设置
    Server_ip = ui->lineEditIP->text();//设置ip
    Server_port = ui->lineEditPort->text().toInt();//设置端口
    qDebug()<<Server_ip<<"  "<<Server_port;
    ui->Settings->setVisible(false);
}

void ClientWidget::closeEvent(QCloseEvent *event)
{
    //在这里添加你希望执行关闭事件需要处理的事情
    //关闭其他窗口
    sw.s_Socket->disconnectFromHost();
    fw.f_Socket->disconnectFromHost();
    tcpSocket->disconnectFromHost();
    sw.close();
    fw.close();
}
