#pragma once
#include<winsock2.h>
#include<iostream>
#include "../net/packDef.h"
#include<set>

using namespace std;
//#pragma comment(lib,"Ws2_32.lib")

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
    //获取本地IP地址列表
    static set<long> GetIpList(){
        set<long> ipList;
        char name[100] = "";
        //获取主机名
        gethostname(name,sizeof(name));

        //使用主机名获取ip地址列表
        struct hostent *remoteHost = gethostbyname(name);
        int i =0;
        unsigned long ip =0;
        while(remoteHost->h_addr_list[i] != 0){
            ip = *(u_long*)remoteHost->h_addr_list[i++];
            ipList.insert(ip);
        }
        return ipList;
    }

    INetMediator* m_pMediator;
protected:
    //接收数据
    virtual void RecvData() = 0;
};
