#ifndef USERITEM_H
#define USERITEM_H

#include <QWidget>
#include<QBitmap>
#include"./net/packdef.h"
#include<QDebug>

namespace Ui {
class UserItem;
}

class UserItem : public QWidget
{
    Q_OBJECT
signals:
    //给kernel发信号，让他来显示与当前好友的聊天窗口
    void SIG_showChatDialog(QString ip);

    void SIG_showTcpChatDlg(int id);
public:
    explicit UserItem(QWidget *parent = 0);
    ~UserItem();

    //设置好友的信息
    void setFriendInfo(int iconId,QString name,QString feeling,int status = status_online,int id = -1);

    //设置好友下线
    void setFriendOffline();

    QString feeling() const;
    void setFeeling(const QString &feeling);

    QString name() const;

private slots:
    void on_pb_icon_clicked();

private:
    Ui::UserItem *ui;
    int m_userId;
    int m_iconId;
    int m_status;
    QString m_name;
    QString m_feeling;
};

#endif // USERITEM_H
