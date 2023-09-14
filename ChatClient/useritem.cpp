#include "useritem.h"
#include "ui_useritem.h"
#include<QBitmap>

UserItem::UserItem(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::UserItem)
{
    ui->setupUi(this);
}

UserItem::~UserItem()
{
    delete ui;
}
//设置好友的信息
void UserItem::setFriendInfo(int iconId, QString name, QString feeling,int status,int id){

    qDebug()<<__func__<<endl;
    //保存好友信息
    m_iconId = iconId;
    m_name = name;
    m_feeling = feeling;
    m_status = status;
    m_userId = id;

    //把好友信息设置到控件上显示
    ui->lb_name->setText(m_name);
    ui->lb_feeling->setText(m_feeling);
    QString path = QString(":/tx/%1.png").arg(m_iconId);

    //判断好友状态
    if(status_online == m_status){
        //在线，头像亮显
        ui->pb_icon->setIcon(QIcon(path));

    }else{
        //下线，头像暗显
        QBitmap bp;
        bp.load(path);
        ui->pb_icon->setIcon(bp);
    }

}

//设置好友下线
void UserItem::setFriendOffline()
{
    //改变好友的在线状态，并且设置头像变灰

    m_status = status_offline;
    QString path = QString(":/tx/%1.png").arg(m_iconId);
    QBitmap bp;
    bp.load(path);
    ui->pb_icon->setIcon(bp);

    //重绘
    repaint();

}

void UserItem::on_pb_icon_clicked()
{
    if(m_userId != -1){
    //TCP:给kernel发消息，让他显示好友的聊天窗口
    Q_EMIT SIG_showTcpChatDlg(m_userId);
    }
    //UDP:给kernel发消息，让他来显示与当前好友的聊天窗口
    Q_EMIT SIG_showChatDialog(m_feeling);

}

QString UserItem::name() const
{
    return m_name;
}

void UserItem::setFeeling(const QString &feeling)
{
    m_feeling = feeling;
}

QString UserItem::feeling() const
{
    return m_feeling;
}
