#pragma once
#include"INet.h"
#include<map>
#include<list>

class TcpServer :public INet {
public:
    TcpServer(INetMediator* pMediator);
    ~TcpServer();
    //初始化网络
    bool InitNet();

    //发送数据(UDP是IP地址，TPC是socket)
    bool SendData(char* sendData, int nLen, long lSend);
    //关闭网络
    void UnInitData();
protected:
    //接收数据
    void RecvData();
private:
    //接收数据的线程函数
    static unsigned __stdcall RecvThread(void* lpVoid);

    //接受连接的线程函数
    static unsigned __stdcall AcceptThread(void* lpVoid);

    SOCKET m_sock;  //TcpServer的私有成员变量
    HANDLE m_handle;
    bool m_bStop;

    //保存客户端（线程id）和socket的对应关系
    map<unsigned int,SOCKET>m_mapThreadIdToSocket;

    //保存线程句柄
    list<HANDLE>m_listHandle;

};
