#ifndef SQLITE_H
#define SQLITE_H

#include <QWidget>
#include <QSqlTableModel>

namespace Ui {
class Sqlite;
}

class Sqlite : public QWidget
{
    Q_OBJECT

public:
    explicit Sqlite(QWidget *parent = 0);
    ~Sqlite();

    bool login(QString username,QString password);
    bool findback(QString username,QString Answer,QString Newpw);
    bool signup(QString username,QString passward,QString Question,QString Answer);
    QString getback(QString username);
    void logout(QString username);
    bool checkchat(QString friendname);
    bool newoff(QString sender,QString reciver,QString message);//插入新的离线消息
    QString findoff(int row,QString reciver);//查找这个接收方的离线消息
    QSqlTableModel *modeloff;

private slots:
    void on_buttonDel_clicked();

    void on_buttonSure_clicked();

    void on_buttonHome_clicked();

private:
    Ui::Sqlite *ui;
    QSqlTableModel *model;

};

#endif // SQLITE_H
