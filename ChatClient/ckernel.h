#ifndef CKERNEL_H
#define CKERNEL_H

#include<QMap>
#include <QObject>
#include"dialog.h"
#include"mediator/INetMediator.h"
#include"chatdialog.h"
#include<QInputDialog>
#include"logindialog.h"
#include"./net/packDef.h"
#include"useritem.h"

//定义函数指针
class CKernel;
typedef void(CKernel::*pFun)(char* recvData, int nLen, long lFrom);

class CKernel : public QObject
{
    Q_OBJECT
public:
    explicit CKernel(QObject *parent = 0);
    ~CKernel();        //声明析构函数

    //utf-8转gb2312
    void utf8ToGb2312(QString src,char* dest,int nLen);

    //gb2312转utf-8
    QString gb2312ToUtf8(char* src);

    //初始化协议头数组
    void setProtocolMap();

    //处理上线请求
    void dealOnlineRq(char* recvData,int nLen,long lFrom);

    //处理上线回复
    void dealOnlineRs(char* recvData,int nLen,long lFrom);

    //处理聊天请求
    void dealChatRq(char* recvData,int nLen,long lFrom);

    //处理下线请求
    void dealOfflineRq(char* recvData,int nLen,long lFrom);

    //处理注册回复
    void dealRegisterRs(char* recvData,int nLen,long lFrom);

    //处理登录回复
    void dealLoginRs(char* recvData,int nLen,long lFrom);

    //处理好友信息
    void dealFriendInfo(char* recvData,int nLen,long lFrom);

    //处理TCP下线请求
    void dealTcpOfflineRq(char* recvData,int nLen,long lFrom);

    //处理TCP聊天请求
    void dealTcpChatRq(char* recvData,int nLen,long lFrom);

    //处理TCP聊天回复
    void dealTcpChatRs(char* recvData,int nLen,long lFrom);

    //处理添加好友请求
    void dealAddFriendRq(char* recvData,int nLen,long lFrom);

    //处理添加好友回复
    void dealAddFriendRs(char* recvData,int nLen,long lFrom);



signals:

public slots:
    //处理所有收到的数据
  void slots_ReadyData(char* recvData,int nLen,long lFrom);

  //显示与当前好友的聊天窗口
  void slot_showChatDialog(QString ip);

  //处理聊天内容
  void slot_sendMsg(QString content,QString ip);

  //处理关闭应用程序
  void slot_close();

  //处理关闭窗口的槽函数
  void slot_closeLoginDlg();

  //TCP 显示与当前好友的聊天窗口
  void slot_showTcpChatDlg(int id);


  //TCP：处理聊天内容
  void slot_sendTcpMsg(QString content,int id);


  //处理注册信息
  void slot_registerCommit(QString name,QString tel,QString password);

  //处理登录信息
  void slot_loginCommit(QString tel,QString password);

  //处理添加好友
  void slot_addFriend();



private:
    int m_id;
    QString m_name;
    Dialog* m_pImDlg;
    LoginDialog* m_LoginDlg;
    INetMediator* m_pMediator;
    INetMediator* m_pTcpMediator;
    //保存UDP聊天窗口
    QMap<long,ChatDialog*> m_mapIpToChatdlg;
    //保存TCP聊天窗口
    QMap<long,ChatDialog*> m_mapIdToChatdlg;
    //定义函数指针数组
    pFun m_mapPackTypeToFun[_DEF_PROTOCOL_COUNT];
    //保存Tcp协议的好友id和useritem
    QMap<int,UserItem*> m_mapIdToUserItem;

};


#endif // CKERNEL_H
