#include "sqlite.h"
#include "ui_sqlite.h"
#include <QSqlDatabase>
#include <QDebug>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlField>

Sqlite::Sqlite(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Sqlite)
{
    ui->setupUi(this);

    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");

    db.setDatabaseName("../info.db");//在上一级创建一个db文件
    if (!db.open()) {
        qDebug() << "Database Error!";
        return;
    }
    QSqlQuery query;//新建表
    query.exec("create table users (username varchar(40), password varchar(40), Question varchar(40), Answer varchar(40), status int)");
    //初始化5个用户
/*    query.prepare("insert into users(username, password, Question, Answer, status) values(?, ?, ?, ?, ?)");//给字段设置内容list
    QVariantList usernameList;
    usernameList << "root" << "one" << "two" << "three" << "four";

    QVariantList passwordList;
    passwordList << "root" << "one" << "two" << "three" << "four";

    QVariantList QuestionList;
    QuestionList << "root" << "one" << "two" << "three" << "four";

    QVariantList AnswerList;
    AnswerList << "root" << "one" << "two" << "three" << "four";
    QVariantList statusList;
    statusList << "0" << "0" << "0" << "0" << "0";
    // 给字段绑定相应的值 按顺序
    query.addBindValue(usernameList);
    query.addBindValue(passwordList);
    query.addBindValue(QuestionList);
    query.addBindValue(AnswerList);
    query.addBindValue(statusList);
*/
    //新建离线信息表
   query.exec("create table offline (sender varchar(40), receiver varchar(40), message varchar(40))");

    //执行预处理命令
    query.execBatch();

    //设置模型
    model = new QSqlTableModel(this);
    //指定使用哪个表
    model->setTable("users");

//把model放在view里
ui->tableView->setModel(model);

    //显示model里的数据
    model->select();
    //设置model的编辑模式
    //model->setEditStrategy(QSqlTableModel::OnManualSubmit);//手动提交修改
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);//不可更改


    modeloff = new QSqlTableModel(this);
    modeloff->setTable("offline");//指定使用哪个表

//把modeloff放在view里
//ui->tableView->setModel(modeloff);

    modeloff->select();

   // newoff("234","234","234");
    //signup("234","234","234","234");

}

Sqlite::~Sqlite()
{
    delete ui;
}

bool Sqlite::login(QString username,QString password)//登录
{
    //开启一个事务
    QSqlDatabase::database().transaction();
    QSqlQuery query;

    int row = 0;
    //qDebug()<<model->rowCount();
    while (row < model->rowCount())
    {

        QSqlRecord record = model->record(row);
        if ((record.value("username") == username)&&(record.value("password") == password))
        {
            //成功登录后status变为1
            query.prepare("UPDATE users SET status=:status where username=:username");
            query.bindValue(":status",1);
            query.bindValue(":username",username);
            query.exec();//执行SQL语句的

            model->select();//把操作后的表显示出来，这句可能容易出错

            QSqlDatabase::database().commit();//提交操作

            return true;
        }
        else
            row++;
   }
   return false;			//如果未查询到则返回false
}

bool Sqlite::signup(QString username,QString password,QString Question,QString Answer)
{
    //开启一个事务
    QSqlDatabase::database().transaction();
    QSqlQuery query;

    int row = 0;
    while (row < model->rowCount())
    {

        QSqlRecord record = model->record(row);
        if ((record.value("username") == username))
            return false;
        else
            row++;
   }
    //如果未查询到则创建
    row=row+1;
    query.prepare("insert into users(username, password, Question, Answer, status) values(:username, :password, :Question, :Answer, :status)");
    query.bindValue(":username",username);
    query.bindValue(":password",password);
    query.bindValue(":Question",Question);
    query.bindValue(":Answer",Answer);
    query.bindValue(":status",0);
    query.exec();

    model->select();//把添加后的表显示出来，这句可能容易出错

    QSqlDatabase::database().commit();//提交操作

/*    query.exec("select * from users");
    while(query.next())//一行一行遍历
    {
        //取出当前行的内容
        qDebug()<< query.value(0).toInt()
                << query.value(1).toString()
                << query.value(2).toString()
                << query.value(3).toString()
                << query.value("Answer").toString();
    }
*/
    return true;
}

bool Sqlite::findback(QString username,QString Answer,QString Newpw)
{

    //开启一个事务
    QSqlDatabase::database().transaction();
    QSqlQuery query;

    int row = 0;
    while (row < model->rowCount())
    {

        QSqlRecord record = model->record(row);
        if ((record.value("username") == username)&&(record.value("Answer") == Answer))
        {
            //查询到对应用户，进行数据更新
            query.prepare("UPDATE users SET password=:password where username=:username");
            query.bindValue(":password",Newpw);
            query.bindValue(":username",username);
            query.exec();//这句是真正执行SQL语句的

            model->select();//把添加后的表显示出来，这句可能容易出错

            QSqlDatabase::database().commit();//提交操作
            return true;
        }
        else
            row++;
   }
    //如果未查询到
    return false;
}

QString Sqlite::getback(QString username)
{

    QString str = "";
    int row = 0;
    while (row < model->rowCount())
    {
        QSqlRecord record = model->record(row);
        if ((record.value("username") == username))
        {
            //查询到对应用户，返回密保问题
            str = record.value("Question").toString();
            return str;
        }
        else
            row++;
   }
    //如果未查询到
    return str;
}

void Sqlite::logout(QString username)//离线，把status变为0
{
    //开启一个事务
    QSqlDatabase::database().transaction();
    QSqlQuery query;
    int row = 0;
    while (row < model->rowCount())
    {
        QSqlRecord record = model->record(row);
        if ((record.value("username") == username))
        {
            query.prepare("UPDATE users SET status=:status where username=:username");
            query.bindValue(":status",0);
            query.bindValue(":username",username);
            query.exec();//执行SQL语句的
            model->select();//把操作后的表显示出来，这句可能容易出错
            QSqlDatabase::database().commit();//提交操作
            return;
        }
        else
            row++;
   }
}

bool Sqlite::checkchat(QString friendname)
{
    int row = 0;
    while (row < model->rowCount())
    {
        QSqlRecord record = model->record(row);
        if ((record.value("username") == friendname))
        {
            if(record.value("status")==1)//在线
                return true;
            else
                return false;
        }
        else
            row++;
   }
    //如果未查询到
    return false;
}

bool Sqlite::newoff(QString sender,QString receiver,QString message)//插入新的离线消息
{
    //开启一个事务
    QSqlDatabase::database().transaction();
    QSqlQuery query;

    query.prepare("insert into offline(sender, receiver, message) values(:sender, :receiver, :message)");
    query.bindValue(":sender",sender);
    query.bindValue(":receiver",receiver);
    query.bindValue(":message",message);
    query.exec();

    modeloff->select();//把添加后的表显示出来

    QSqlDatabase::database().commit();//提交操作

/*    query.exec("select * from offline");
       while(query.next())//一行一行遍历
       {
           //取出当前行的内容
           qDebug()<< query.value(0).toInt()
                   << query.value(1).toString()
                   << query.value(2).toString()
                   << query.value(3).toString();
       }
*/
    return true;
}

QString Sqlite::findoff(int row,QString receiver)//查找这个接收方的离线消息
{
    QString str;
    QSqlRecord record = modeloff->record(row);//获取本行消息
    if(record.value("receiver") == receiver)//
    {
        str = record.value("sender").toString();//发送方
        str+="#";
        str+= record.value("message").toString();//信息
        return str;
    }
    else
        return "";

}


void Sqlite::on_buttonDel_clicked()
{
    QItemSelectionModel *sMode1 = ui->tableView->selectionModel();//获取选中的模型
    //取出模型中的索引
    QModelIndexList list = sMode1->selectedRows();
    //删除所有行
    for(int i = 0;i<list.size();i++)
    {
        model->removeRow(list.at(i).row());
        //modeloff->removeRow(list.at(i).row());
    }
}

void Sqlite::on_buttonSure_clicked()
{
    QSqlDatabase::database().commit();
    model->select();//把操作后的表显示出来，这句可能容易出错
    //modeloff->select();
}

void Sqlite::on_buttonHome_clicked()
{
    this->hide();
    model->select();
}

