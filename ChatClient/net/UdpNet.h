#pragma once
#include"INet.h"

class UdpNet:public INet{
public:
    UdpNet(INetMediator* pMediator);
    ~UdpNet();
    //初始化网络
    bool InitNet();

    //发送数据(UDP是IP地址，TPC是socket)
    bool SendData(char* sendData, int nLen, long lSend);
    //关闭网络
    void UnInitData();
protected:
    //接收数据
    void RecvData() ;
private:
    //接收数据的线程函数
    static unsigned __stdcall RecvThread(void* lpVoid);
    SOCKET m_sock;  //UdpNet的私有成员变量U
    HANDLE m_handle;
    bool m_bStop;



};
