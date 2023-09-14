#pragma once
#include<winsock2.h>
#include<iostream>
#include "../net/packDef.h"

using namespace std;
#pragma comment(lib,"Ws2_32.lib")

class INetMediator;
class INet {
public:
    INet() {}
    virtual ~INet() {}
    //初始化网络
    virtual bool InitNet() = 0;

    //发送数据(UDP是IP地址，TCP是socket)
    virtual bool SendData(char* sendData, int nLen, long ISend) = 0;

    //关闭网络
    virtual void UnInitData() = 0;


protected:
    //接收数据
    virtual void RecvData() = 0;

    INetMediator* m_pMediator;
};
