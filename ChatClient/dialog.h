#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include<QCloseEvent>
#include<QVBoxLayout>
#include"useritem.h"
#include<QMap>
#include<QMenu>
#include<QAction>

namespace Ui {
class Dialog;
}

class Dialog : public QDialog
{
    Q_OBJECT
signals:
    //通知kernel关闭应用程序
    void SIG_close();

    //给kernel发送添加好友的信号
    void SIG_addFriend();

public:
    explicit Dialog(QWidget *parent = 0);
    ~Dialog();

    //设置用户信息到界面上
    void setUserInfo(int iconId,QString name,QString feeling,int id);

    //添加好友
    void addFriend(long ip, UserItem* item);

    //添加Tcp好友
    void addTcpFriend(UserItem* item);

    //删除好友
    void deleteFriend(long ip);

    //删除Tcp好友
    void deleteTcpFriend(UserItem* item);

    //重写关闭窗口事件
    void closeEvent(QCloseEvent * event);

private slots:
    void on_pb_menu_clicked();

    //点击菜单栏的槽函数
    void slot_clickMenu(QAction* action);

private:
    Ui::Dialog *ui;
    //udp的层
    QVBoxLayout* m_pUdplayout;
    //tcp的层
    QVBoxLayout* m_pTcplayout;
    QMap<long,UserItem*> m_mapIpToUseritem;

    //菜单栏
    QMenu* m_menu;
};

#endif // DIALOG_H
