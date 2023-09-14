#include "dialog.h"
#include "ui_dialog.h"
#include<QMessageBox>

Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);

    //设置窗口的title
    setWindowTitle("IM聊天系统");

    //new一个垂直布局的layout
    m_pUdplayout = new QVBoxLayout;
    //设置控件间的间隔
    m_pUdplayout->setSpacing(3);
    //设置小控件和大控件之间的距离
    m_pUdplayout->setContentsMargins(3,0,3,0);
    //把layout设置到大控件上
    ui->wdg_list->setLayout(m_pUdplayout);

    //new一个垂直布局的layout
    m_pTcplayout = new QVBoxLayout;
    //设置控件间的间隔
    m_pTcplayout->setSpacing(3);
    //设置小控件和大控件之间的距离
    m_pTcplayout->setContentsMargins(3,0,3,0);
    //把layout设置到大控件上
    ui->wdg_tcp_list->setLayout(m_pTcplayout);

    //给菜单栏new一个对象
    m_menu = new QMenu;
    //添加菜单项
    m_menu->addAction("Add friend");
    //m_menu->addAction("Add Group");

    //绑定点击菜单栏的信号和槽函数（信号来自于QMenu中的triggered）
    connect(m_menu,SIGNAL(triggered(QAction*)),
            this,SLOT(slot_clickMenu(QAction*)));

}

Dialog::~Dialog()
{
    delete ui;
}

//设置用户信息到界面上
void Dialog::setUserInfo(int iconId, QString name, QString feeling,int id)
{
    QString path = QString(":/tx/%1.png").arg(iconId);
    ui->pb_icon->setIcon(QIcon(path));
    ui->lb_name->setText(name);
    ui->ld_feeling->setText(feeling);

}

//添加好友
void Dialog::addFriend(long ip, UserItem *item)
{
    m_pUdplayout->addWidget(item);
}

//添加Tcp好友
void Dialog::addTcpFriend(UserItem *item)
{
    m_pTcplayout->addWidget(item);
}

//删除好友
void Dialog::deleteFriend(long ip)
{
    auto ite = m_mapIpToUseritem.find(ip);
    if(ite != m_mapIpToUseritem.end()){
        UserItem* item = *ite;
        if(item){
            //隐藏控件
            item->hide();

            //从列表上移除
            m_pUdplayout->removeWidget(item);

            //把父控件设置为空
            item->setParent(nullptr);
            delete item;
            item = nullptr;

        }
        //把无效节点从map中移除
        m_mapIpToUseritem.erase(ite);
    }
}

//删除Tcp好友
void Dialog::deleteTcpFriend(UserItem *item)
{

}


//重写关闭窗口事件
void Dialog::closeEvent(QCloseEvent *event)
{
    //忽略用户操作，不走父类的关闭
    event->ignore();
    //询问用户是否确定关闭窗口
    if(QMessageBox::Yes == QMessageBox::question(this,"提示","是否退出登录")){
        //确定关闭窗口，通知kernel
        Q_EMIT SIG_close();
    }
}

//在鼠标点击位置向上显示菜单栏
void Dialog::on_pb_menu_clicked()
{
    //获取鼠标点击位置
    QPoint pos = QCursor::pos();
    //获取菜单栏大小
    QSize size = m_menu->sizeHint();
    //显示菜单栏，让y坐标向上移动菜单栏高度以达到向上显示的效果
    m_menu->exec(QPoint(pos.x(),pos.y() - size.height()));

}

//点击菜单栏的槽函数
void Dialog::slot_clickMenu(QAction *action)
{
    if(action->text() == "Add friend"){
        //给kernel发送添加好友的信号
        Q_EMIT SIG_addFriend();
    }else if(action->text() == "Add group"){
        qDebug()<<"Add group";

    }


}
