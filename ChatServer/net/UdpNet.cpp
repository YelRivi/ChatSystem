#include"UdpNet.h"
#include<process.h>
#include"../mediator/UdpNetMediator.h"


UdpNet::UdpNet(INetMediator* pMediator) :m_sock(INVALID_SOCKET), m_handle(NULL), m_bStop(false) {
    m_pMediator = pMediator;
}

UdpNet::~UdpNet(){

}

//初始化网络：加载库，创建套接字，绑定IP地址，创建接收数据的线程
bool UdpNet::InitNet(){

    //1.加载库
    int err = 0;
    WORD version = MAKEWORD(2, 2);
    WSADATA wsaData;
    err = WSAStartup(version, &wsaData);

    //判断返回值
     if (0 != err) {
         cout << "WSAStartup error: " << endl;
         return false;
    }
     //判断版本号
     if (2 != HIBYTE(wsaData.wHighVersion) || 2 != LOBYTE(wsaData.wVersion)){
         cout<<"WSAStartup version error."<<endl;
             return false;
     }
     else {
         cout<<"WSAStartup success."<<endl;
     }
    //2.创建套接字
     m_sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);//ipv4,m_sock协议，udp协议
        if (INVALID_SOCKET == m_sock) {
                 cout << "m_socket error:" << WSAGetLastError() << endl;
                 return false;
             }
         else {
                 cout << "socket success." << endl;
             }
         
    //3.绑定IP地址
         sockaddr_in addr;
         addr.sin_family = AF_INET;
         addr.sin_port = htons(_DEF_UDP_PORT);
         addr.sin_addr.S_un.S_addr = INADDR_ANY;
         err = bind(m_sock, (sockaddr*)&addr, sizeof(addr));
         if (SOCKET_ERROR == err) {
             cout << "bind error" << WSAGetLastError() << endl;
             return false;
         }
         else {
             cout << "bind success." << endl;
         }
         //申请广播权限
         bool val = true;
         setsockopt(m_sock, SOL_SOCKET, SO_BROADCAST, (char*)&val, sizeof(val));
    //创建接收数据的线程
         m_handle=(HANDLE)_beginthreadex(0,0,&RecvThread,this,0,NULL);
         if (!m_handle) {
             cout << "_beginthreadex error" << endl;
             return false;
         }
         else {
             cout << "_beginthreadex success" << endl;
         }

    return true;
}
//接收数据的线程函数

unsigned __stdcall UdpNet::RecvThread(void* lpVoid) {
    UdpNet* pThis = (UdpNet*)lpVoid;
    pThis->RecvData();
    return 1;
}

//发送数据(UDP是IP地址，TCP是m_socket)
    int nSendNum = 0;
//char sendBuf[4096] = "";
bool UdpNet::SendData(char* sendData, int nLen, long lSend){
    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(_DEF_UDP_PORT);
    addr.sin_addr.S_un.S_addr = lSend;
    //nSendNum = sendto(m_sock,sendData,nLen,0,addr,sizeof(addr));
    if (sendto(m_sock, sendData,nLen,0,(sockaddr*)&addr,sizeof(addr))<= 0) {
        cout << "send error:" << WSAGetLastError() << endl;
        return false;
    }
    return true;
}

//接收数据
void UdpNet::RecvData() {
    
    int nRecvNum = 0;
    char recvBuf[4096] = "";
    sockaddr_in addr;
    int addrSize = sizeof(addr);
    while (!m_bStop) {
        nRecvNum = recvfrom(m_sock,recvBuf,sizeof(recvBuf),0,(sockaddr*)&addr,&addrSize);
        if (nRecvNum > 0) {
            //cout << "UdpNet::RecvData ip:" << inet_ntoa(addr.sin_addr) << ":" << recvBuf << endl;
            m_pMediator->DealData(recvBuf, nRecvNum, addr.sin_addr.S_un.S_addr);
        }
        else {
            cout << "recvform error:" << WSAGetLastError << endl;
            break;
        }
    }
}

//关闭网络:回收线程资源（回收内核对象，1、结束线程工作，2、关闭句柄）关闭套接字，卸载库

void UdpNet::UnInitData() {
    //1.回收线程资源（回收内核对象，1、结束线程工作，2、关闭句柄）
    m_bStop = true;
    if (m_handle) {
        if (WAIT_TIMEOUT == WaitForSingleObject(m_handle,500)) {
            TerminateThread(m_handle, -1);
        }
        CloseHandle(m_handle);
    }
    //2.关闭套接字,卸载库
    if (m_sock && INVALID_SOCKET != m_sock) {
        closesocket(m_sock);
    }
    WSACleanup();

    
}
