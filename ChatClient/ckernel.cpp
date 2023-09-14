#include "ckernel.h"
#include"mediator/UdpNetMediator.h"
#include"mediator/TcpClientMediator.h"
#include<QMessageBox>
#include<QTextCodec>
#include<QDebug>
#include"net/INet.h"

CKernel::CKernel(QObject *parent) : QObject(parent),m_id(0)
{
    //初始化协议头数组
    setProtocolMap();


    //new一个登录&注册窗口并显示
    m_LoginDlg = new LoginDialog;
    m_LoginDlg->showNormal();

    //绑定注册信息的槽和槽函数
    connect(m_LoginDlg,SIGNAL(SIG_registerCommit(QString, QString, QString)),
            this,SLOT(slot_registerCommit(QString, QString, QString )));

    //绑定登录信息的槽和槽函数
    connect(m_LoginDlg,SIGNAL(SIG_loginCommit(QString, QString)),
            this,SLOT(slot_loginCommit(QString, QString )));


    //绑定关闭窗口的信号和槽函数
    connect(m_LoginDlg,SIGNAL(SIG_closeLoginDlg()),
            this,SLOT(slot_closeLoginDlg()));

    //new一个窗口对象
        m_pImDlg = new Dialog;
        //m_pImDlg->show();

    //绑定关闭窗口的信号和槽函数
    connect(m_pImDlg,SIGNAL(SIG_close()),this,SLOT(slot_close()));


    //绑定添加好友的信号和槽函数
    connect(m_pImDlg,SIGNAL(SIG_addFriend()),
            this,SLOT(slot_addFriend()));

    //new一个中介者类对象
    m_pMediator = new UdpNetMediator;



    //打开网络
//    if(!m_pMediator->OpenNet()){
//        QMessageBox::about(m_pImDlg,"提示","网络连接失败");
//        exit(0);
//    }
    //绑定处理接收数据的信号和槽函数
    connect(m_pMediator,SIGNAL(SIG_ReadyData(char*,int,long))
            ,this,SLOT(slots_ReadyData(char*,int,long)));


    //广播发个上线请求结构体,通知大家我上线了
    struct _STRU_ONLINE rq;
    gethostname(rq.name,sizeof(rq));
     m_pMediator->SendData((char*)&rq,sizeof(rq),inet_addr("255.255.255.255"));

     //new一个tcp中介者类对象
     m_pTcpMediator = new TcpClientMediator;

     //打开网络
     if(!m_pTcpMediator->OpenNet()){
         QMessageBox::about(m_pImDlg,"message","Open Net Fail...");
         exit(0);
     }

     //绑定处理接收到的所有TCP数据的信号和槽函数
     connect(m_pTcpMediator,SIGNAL(SIG_ReadyData(char*,int,long))
             ,this,SLOT(slots_ReadyData(char*,int,long)));

}

CKernel::~CKernel(){
    //清理弹出框的空间
    //回收资源
    if(m_pImDlg){
        m_pImDlg->hide();
        delete m_pImDlg;
        m_pImDlg = nullptr;
    }

    if(m_pMediator){
        m_pMediator->CloseNet();
        delete m_pMediator;
        m_pMediator = nullptr;
    }

    for(auto ite = m_mapIpToChatdlg.begin();ite != m_mapIpToChatdlg.end();){
        ChatDialog* chat = *ite;
        if(chat){
            chat->hide();
            delete chat;
            chat = nullptr;
        }

        //从map中移除无效结点
        ite = m_mapIpToChatdlg.erase(ite);//移除后会自动指向下一个结点，所以for循环中无需++
    }

}

//UTF-8转GB2312
void CKernel::utf8ToGb2312(QString src, char *dest, int nLen)
{
    QTextCodec* dc = QTextCodec::codecForName("gb2312");
    QByteArray ba =dc->fromUnicode(src);
    if(ba.length() <= nLen){
        strcpy(dest,ba.data());
    }else{
        qDebug()<<"src is too long.";
    }
}
//GB2312转UTF-8
QString CKernel::gb2312ToUtf8(char *src)
{
    QTextCodec* dc = QTextCodec::codecForName("gb2312");
    return dc->toUnicode(src);
}

//初始化协议头数组
void CKernel::setProtocolMap()
{
    qDebug()<<__func__;
    //数组初始化成0
    memset(m_mapPackTypeToFun,0,sizeof(m_mapPackTypeToFun));
    //绑定协议头和处理函数
    m_mapPackTypeToFun[_DEF_UDP_ONLINE_RQ - _DEF_PROTOCOL_BASE - 1]   = &CKernel::dealOnlineRq;
    m_mapPackTypeToFun[_DEF_UDP_ONLINE_RS - _DEF_PROTOCOL_BASE - 1]   = &CKernel::dealOnlineRs;
    m_mapPackTypeToFun[_DEF_UDP_CHAT_RQ - _DEF_PROTOCOL_BASE - 1]     = &CKernel::dealChatRq;
    m_mapPackTypeToFun[_DEF_UDP_OFFLINE_RQ - _DEF_PROTOCOL_BASE - 1]  = &CKernel::dealOfflineRq;
    m_mapPackTypeToFun[_DEF_TCP_REGISTER_RS - _DEF_PROTOCOL_BASE - 1] = &CKernel::dealRegisterRs;
    m_mapPackTypeToFun[_DEF_TCP_LOGIN_RS - _DEF_PROTOCOL_BASE - 1]    = &CKernel::dealLoginRs;
    m_mapPackTypeToFun[_DEF_TCP_FRIEND_INFO - _DEF_PROTOCOL_BASE - 1] = &CKernel::dealFriendInfo;
    m_mapPackTypeToFun[_DEF_TCP_OFFLINE_RQ - _DEF_PROTOCOL_BASE - 1]  = &CKernel::dealTcpOfflineRq;
    m_mapPackTypeToFun[_DEF_TCP_CHAT_RQ - _DEF_PROTOCOL_BASE -1]      = &CKernel::dealTcpChatRq;
    m_mapPackTypeToFun[_DEF_TCP_CHAT_RS - _DEF_PROTOCOL_BASE -1]      = &CKernel::dealTcpChatRs;
    m_mapPackTypeToFun[_DEF_TCP_ADD_FRIEND_RQ - _DEF_PROTOCOL_BASE -1]= &CKernel::dealAddFriendRq;
    m_mapPackTypeToFun[_DEF_TCP_ADD_FRIEND_RS - _DEF_PROTOCOL_BASE -1]= &CKernel::dealAddFriendRs;

}
//UDP:处理上线请求
void CKernel::dealOnlineRq(char *recvData, int nLen, long lFrom)
{
    qDebug()<<__func__;
    //1、拆包
    struct _STRU_ONLINE* rq =(_STRU_ONLINE*)recvData;

    //2、添加到好友列表上
    //new一个好友
    UserItem* item = new UserItem;
    struct in_addr addr;
    addr.S_un.S_addr = lFrom;
    item->setFriendInfo(14,rq->name,inet_ntoa(addr));
    //把好友添加到列表上
    m_pImDlg->addFriend(lFrom,item);

    //绑定显示聊天窗口的信号和槽函数
    connect(item,SIGNAL(SIG_showChatDialog(QString)),
            this,SLOT(slot_showChatDialog(QString)));

    //new一个和这个人的聊天窗口
    ChatDialog* chat = new ChatDialog;
    //设置窗口信息
    chat->setChatDialogInfo(inet_ntoa(addr));
    //保存到map中
    m_mapIpToChatdlg[lFrom] = chat;
    connect(chat,SIGNAL(SIG_sendMsg(QString,QString)),
            this,SLOT(slot_sendMsg(QString,QString)));

    //3、判断是不是自己的广播
    //获取本地ip地址列表
    set<long> ipList = INet::GetIpList();
    //
    if(ipList.find(lFrom)!=ipList.end()){
        return;
    }
    //4、写一个回复包
    struct _STRU_ONLINE rs;
    rs.type = _DEF_UDP_ONLINE_RS;
    gethostname(rs.name,sizeof(rs));
    m_pMediator->SendData((char*)&rs,sizeof(rs),lFrom);

}
//UDP:处理上线回复
void CKernel::dealOnlineRs(char *recvData, int nLen, long lFrom)
{
    //1、拆包
    struct _STRU_ONLINE* rq =(_STRU_ONLINE*)recvData;

    //2、添加到好友列表上

    //new一个好友
    UserItem* item = new UserItem;
    struct in_addr addr;
    addr.S_un.S_addr = lFrom;

    //设置好友属性
    item->setFriendInfo(14,rq->name,inet_ntoa(addr));

    //把好友添加到列表上
    m_pImDlg->addFriend(lFrom,item);

    //绑定显示聊天窗口的信号和槽函数
    connect(item,SIGNAL(SIG_showChatDialog(QString)),
            this,SLOT(slot_showChatDialog(QString)));

    //new一个和这个人的聊天窗口
    ChatDialog* chat = new ChatDialog;
    //设置窗口信息
    chat->setChatDialogInfo(inet_ntoa(addr));
    //保存到map中
    m_mapIpToChatdlg[lFrom] = chat;
    //绑定发送聊天内容的信号和槽
    connect(chat,SIGNAL(SIG_sendMsg(QString,QString)),
            this,SLOT(slot_sendMsg(QString,QString)));

        qDebug()<<__func__;
}

//UDP:处理聊天请求
void CKernel::dealChatRq(char *recvData, int nLen, long lFrom)
{
            qDebug()<<__func__;
    //1、拆包
    struct _STRU_CHAT_RQ* rq =(_STRU_CHAT_RQ*)recvData;

    //2、找到和这个人的聊天窗口
    if(m_mapIpToChatdlg.count(lFrom) > 0){
        ChatDialog* chat = m_mapIpToChatdlg[lFrom];

        //把聊天内容设置到窗口上
        chat->setContent(rq->content);
        //显示聊天窗口
        chat->showNormal();
        }
}

//UDP:处理下线请求
void CKernel::dealOfflineRq(char *recvData, int nLen, long lFrom)
{
        qDebug()<<__func__;
    //1、拆包
    struct _STRU_OFFLINE_RQ* rq = (_STRU_OFFLINE_RQ*)recvData;

    //2、回收和这个人的聊天窗口（并从map中删除结点）
    auto ite = m_mapIpToChatdlg.find(lFrom);
    if(ite != m_mapIpToChatdlg.end()){
            ChatDialog* chat = *ite;
            if(chat){
                chat->hide();
                delete chat;
                chat = nullptr;
            }

            //从map中移除无效结点
            m_mapIpToChatdlg.erase(ite);
        }
    //3、从列表上删除
    struct in_addr addr;
    addr.S_un.S_addr = lFrom;
    m_pImDlg->deleteFriend(lFrom);
}

//处理注册回复
void CKernel::dealRegisterRs(char *recvData, int nLen, long lFrom)
{
    qDebug() << __func__;
    //拆包
    _STRU_TCP_REGISTER_RS* rs = (_STRU_TCP_REGISTER_RS*)recvData;

    //根据注册结果显示提示信息
    switch(rs->result){
    case register_success:
        QMessageBox::about(m_LoginDlg,"提示","注册成功");
        break;
    case register_tel_used:
        QMessageBox::warning(m_LoginDlg,"提示","该电话号码已被使用");
        break;
    case register_name_used:
        QMessageBox::warning(m_LoginDlg,"提示","该昵称已被使用");
        break;
    default:
        break;
    }
}
//处理登录回复
void CKernel::dealLoginRs(char *recvData, int nLen, long lFrom)
{
    qDebug() << __func__;
    //拆包
    _STRU_TCP_LOGIN_RS* rs = (_STRU_TCP_LOGIN_RS*)recvData;
    switch(rs->result){
    case login_success:
    {
        //跳转界面
        m_LoginDlg->hide();
        m_pImDlg->showNormal();
        //保存用户id
        m_id = rs->userId;
    }
        break;
    case login_no_tel:
        QMessageBox::warning(m_LoginDlg,"提示","请输入正确的号码");
        break;
    case login_password_error:
        QMessageBox::warning(m_LoginDlg,"提示","密码错误");
        break;
    default:
        break;
    }
}

//处理好友信息
void CKernel::dealFriendInfo(char *recvData, int nLen, long lFrom)
{
    qDebug() << __func__;
    //拆包
    _STRU_TCP_FRIEND_INFO* rs = (_STRU_TCP_FRIEND_INFO*)recvData;
    QString name = gb2312ToUtf8(rs->name);
     QString feeling = gb2312ToUtf8(rs->feeling);

    //判断是不是自己
    if(rs->id == m_id){

        //保存自己的信息
        m_name = name;

        //把自己的信息设置到列表的上方
        m_pImDlg->setUserInfo(rs->iconId,name,feeling,rs->id);
        return;
    }


    //判断好友是否已存在于列表上
    if(m_mapIdToUserItem.count(rs->id) > 0){
        //存在，更新好友的信息
        UserItem * item = m_mapIdToUserItem[rs->id];
        item->setFriendInfo(rs->iconId,name,feeling,rs->status,rs->id);
    }else{
        //不存在，new一个好友
        UserItem* item = new UserItem;
        //设置useritem的信息
        item->setFriendInfo(rs->iconId,name,feeling,rs->status,rs->id);
        //把好友加到列表上
        m_pImDlg->addTcpFriend(item);
        //把useritem保存到map中
        m_mapIdToUserItem[rs->id] = item;

        //绑定显示聊天窗口的信号和槽函数
        connect(item,SIGNAL(SIG_showTcpChatDlg(int)),
                this,SLOT(slot_showTcpChatDlg(int)));

        //new一个和该好友的聊天窗口
        ChatDialog* chat = new ChatDialog;
        //设置聊天窗口的信息
        chat->setChatDialogInfo(name,rs->id);
        //聊天窗口保存在map中
        m_mapIdToChatdlg[rs->id] = chat;

        //绑定处理聊天窗口的信号槽函数
        connect(chat,SIGNAL(SIG_sendTcpMsg(QString,int)),
                this,SLOT(slot_sendTcpMsg(QString,int)));
    }

}

//处理TCP下线请求
void CKernel::dealTcpOfflineRq(char *recvData, int nLen, long lFrom)
{
    qDebug() << __func__;
    //拆包
    _STRU_TCP_OFFLINE_RQ* rq = (_STRU_TCP_OFFLINE_RQ*) recvData;

    //判断是否有这个好友的item

    if(m_mapIdToUserItem.count(rq->userId) > 0){
        UserItem* item = m_mapIdToUserItem[rq->userId];
        //改变好友的在线状态，并且设置头像变灰
        item->setFriendOffline();
    }

}

//处理TCP聊天请求
void CKernel::dealTcpChatRq(char *recvData, int nLen, long lFrom)
{
    qDebug() << __func__;
    //拆包
    _STRU_TCP_CHAT_RQ* rq = (_STRU_TCP_CHAT_RQ*) recvData;

    //找到聊天窗口
    if(m_mapIdToChatdlg.count(rq->userId) > 0){
        ChatDialog* chat = m_mapIdToChatdlg[rq->userId];
        //把聊天内容设置到窗口上
        chat->setContent(rq->content);
        //显示聊天窗口
        chat->showNormal();
    }

}

//处理TCP聊天回复
void CKernel::dealTcpChatRs(char *recvData, int nLen, long lFrom)
{
    qDebug() << __func__;
    //拆包
    _STRU_TCP_CHAT_RS* rs = (_STRU_TCP_CHAT_RS*) recvData;

    //找到聊天窗口
    if(m_mapIdToChatdlg.count(rs->friendId) > 0){

        ChatDialog* chat = m_mapIdToChatdlg[rs->friendId];

        //把好友设置到窗口上
        chat->setContent(QString("好友 %1 不在线").arg(chat->getName()));
        //显示聊天窗口
        chat->showNormal();
    }

}

//处理添加好友请求
void CKernel::dealAddFriendRq(char *recvData, int nLen, long lFrom)
{
    qDebug()<< __func__ ;
    //拆包
    _STRU_TCP_ADD_FRIEND_RQ* rq = (_STRU_TCP_ADD_FRIEND_RQ*)recvData;

    //弹出提示框询问用户是否同意添加好友
    _STRU_TCP_ADD_FRIEND_RS rs;
    QString name = QString(rq->userName);

   if( QMessageBox::Yes == QMessageBox::question(m_pImDlg,"好友请求",
        QString("是否同意 %1 添加您为好友？").arg(name)))
   {
       rs.result = add_friend_agree;
   }else{
       rs.result = add_friend_refuse;
   }
   rs.friendId = m_id;
   strcpy(rs.friendName,m_name.toStdString().c_str());
   rs.userId = rq->userId;
   m_pTcpMediator->SendData((char*)&rs,sizeof(rs),23);
}

//处理添加好友回复
void CKernel::dealAddFriendRs(char *recvData, int nLen, long lFrom)
{
    qDebug()<< __func__ ;
    //拆包
    _STRU_TCP_ADD_FRIEND_RS* rs = (_STRU_TCP_ADD_FRIEND_RS*)recvData;
     QString friendName = gb2312ToUtf8(rs->friendName);

    //根据添加结果提示用户
    switch(rs->result){
    //找不到该用户
    case add_friend_no_user:
        QMessageBox::about(m_pImDlg,"提示",QString("找不到 %1 用户").arg(friendName));
        break;

    //该用户是下线状态
    case add_friend_offline:
        QMessageBox::about(m_pImDlg,"提示",QString("用户 %1 未上线").arg(friendName));
        break;

    //用户同意添加
    case add_friend_agree:
        QMessageBox::about(m_pImDlg,"提示",QString("用户 %1 添加成功").arg(friendName));
        break;

    //用户拒绝添加
    case add_friend_refuse:
        QMessageBox::about(m_pImDlg,"提示",QString("用户 %1 拒绝了添加请求").arg(rs->friendName));
        break;

    default:
        break;
    }

}

//处理所有收到的数据
void CKernel::slots_ReadyData(char* recvData,int nLen,long lFrom){
    qDebug() << __func__ ;
    //取出协议头
    PackType type = *(PackType*)recvData;//为什么？
    //根据协议头走到不同的处理流程
    //计算数组下标
    int index = type - _DEF_PROTOCOL_BASE - 1;
    //校验数组下标是否在合法范围内
    if(0 <= index && index < _DEF_PROTOCOL_COUNT){
        pFun pf = m_mapPackTypeToFun[index];
        if(pf){
            (this->*pf)(recvData, nLen, lFrom);
        }else{
            qDebug() << "type:"<< type;
        }
    }else{
        qDebug() << "type:"<< type;
    }

    //删除掉申请的空间
        delete[] recvData;

}

//显示与当前好友的聊天窗口
void CKernel::slot_showChatDialog(QString ip){

    //根据ip找到聊天窗口并显示
    long lip = inet_addr(ip.toStdString().c_str());
    if(m_mapIpToChatdlg.count() > 0){
        ChatDialog* chat = m_mapIpToChatdlg[lip];
        chat->show();
    }
}

//处理聊天内容
void CKernel::slot_sendMsg(QString content, QString ip)
{
    //写一个聊天请求
    struct _STRU_CHAT_RQ rq;
    memcpy(rq.content,content.toStdString().c_str(),content.length());

    //发给对端
    long lIp = inet_addr(ip.toStdString().c_str());
    m_pMediator->SendData((char*)&rq,sizeof(rq),lIp);
}

//处理关闭应用程序
void CKernel::slot_close()
{
    //UDP:发个下线结构体广播，通知大家我下线了
    struct _STRU_OFFLINE_RQ rq;
    m_pMediator->SendData((char*)&rq,sizeof(rq),inet_addr("255.255.255.255"));

    //TCP:通知服务端我下线了
    _STRU_TCP_OFFLINE_RQ tcpRq;
    tcpRq.userId = m_id;
    m_pTcpMediator->SendData((char*)&tcpRq,sizeof(tcpRq),89);

    //回收资源、退出进程
    slot_closeLoginDlg();
}

//处理关闭窗口的槽函数
void CKernel::slot_closeLoginDlg()
{
    //回收资源
    if(m_pImDlg){
        m_pImDlg->hide();
        delete m_pImDlg;
        m_pImDlg = nullptr;
    }
    if(m_LoginDlg){
        m_LoginDlg->hide();
        delete m_LoginDlg;
        m_LoginDlg = nullptr;
    }
    if(m_pMediator){
        m_pMediator->CloseNet();
        delete m_pMediator;
        m_pMediator = nullptr;
    }
    if(m_pTcpMediator){
        m_pTcpMediator->CloseNet();
        delete m_pTcpMediator;
        m_pTcpMediator = nullptr;
    }

    for(auto ite = m_mapIpToChatdlg.begin();ite != m_mapIpToChatdlg.end();){
        ChatDialog* chat = *ite;
        if(chat){
            chat->hide();
            delete chat;
            chat = nullptr;
        }
        //把无效结点从map中移除
       ite = m_mapIpToChatdlg.erase(ite);
    }

    for(auto ite = m_mapIdToChatdlg.begin();ite != m_mapIdToChatdlg.end();){
        ChatDialog* chat = *ite;
        if(chat){
            chat->hide();
            delete chat;
            chat = nullptr;
        }
        //把无效结点从map中移除
       ite = m_mapIdToChatdlg.erase(ite);
    }

    //退出进程
    exit(0);
}

  //TCP 显示与当前好友的聊天窗口
void CKernel::slot_showTcpChatDlg(int id)
{
    //查看map是否有这个好友的聊天窗口，有无显示
    if(m_mapIdToChatdlg.count(id) > 0){
        ChatDialog* chat = m_mapIdToChatdlg[id];
        chat->showNormal();
    }
}


//TCP：处理聊天内容
void CKernel::slot_sendTcpMsg(QString content, int id)
{
    qDebug() << __func__;
    //打包
    _STRU_TCP_CHAT_RQ rq;
    rq.userId = m_id;
    rq.friendId = id;
    strcpy(rq.content,content.toStdString().c_str());

    //发给服务器
    m_pTcpMediator->SendData((char*)&rq,sizeof(rq),12);
}

//处理注册信息
void CKernel::slot_registerCommit(QString name, QString tel, QString password)
{
    qDebug()<< __func__ ;
    //1、打包
    _STRU_TCP_REGISTER_RQ rq;
    utf8ToGb2312(name,rq.name,sizeof(rq.name));
    strcpy(rq.password, password.toStdString().c_str());
    strcpy(rq.tel, tel.toStdString().c_str());

    //2、发给服务端
    m_pTcpMediator->SendData((char*)&rq,sizeof(rq),89);
}

//处理登录信息的槽函数
void CKernel::slot_loginCommit(QString tel, QString password)
{
    qDebug()<< __func__;
    //1、打包
    _STRU_TCP_LOGIN_RQ rq;
    strcpy(rq.password, password.toStdString().c_str());
    strcpy(rq.tel, tel.toStdString().c_str());

    //2、发给服务端
    m_pTcpMediator->SendData((char*)&rq,sizeof(rq),89);
}

//处理添加好友
void CKernel::slot_addFriend()
{
    qDebug()<< __func__;
    //1、弹出窗口，提示用户输入好友信息
    QString name = QInputDialog::getText(m_pImDlg,"添加好友","请输入好友昵称");
    QString nameTmp = name;
    //2、判断输入昵称是否合法
    if(name.isEmpty() || nameTmp.remove(" ").isEmpty()){
        QMessageBox::about(m_pImDlg,"提示","不能输入空格");
        return;
    }
    //3、判断是不是自己
    if(m_name == name){
        QMessageBox::about(m_pImDlg,"提示","不能添加自己！");
        return;
    }
    //4、判断是不是已经是好友
    for(auto ite = m_mapIdToUserItem.begin();ite != m_mapIdToUserItem.end();ite++){
        UserItem* item = *ite;
        if(item->name() == name){
            QMessageBox::about(m_pImDlg,"提示","你们已经是好友啦");
            return;
        }
    }

    //5、给服务端发送添加好友请求
    _STRU_TCP_ADD_FRIEND_RQ rq;
    rq.userId = m_id;
    strcpy(rq.userName,m_name.toStdString().c_str());
    utf8ToGb2312(name,rq.friendName,sizeof(rq.friendName));
    m_pTcpMediator->SendData((char*)&rq,sizeof(rq),89);
}

