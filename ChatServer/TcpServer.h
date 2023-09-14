#pragma once
#include"INet.h"
#include<map>
#include<list>

class TcpServer :public INet {
public:
    TcpServer(INetMediator* pMediator);
    ~TcpServer();
    //��ʼ������
    bool InitNet();

    //��������(UDP��IP��ַ��TPC��socket)
    bool SendData(char* sendData, int nLen, long lSend);
    //�ر�����
    void UnInitData();
protected:
    //��������
    void RecvData();
private:
    //�������ݵ��̺߳���
    static unsigned __stdcall RecvThread(void* lpVoid);

    //�������ӵ��̺߳���
    static unsigned __stdcall AcceptThread(void* lpVoid);

    SOCKET m_sock;  //TcpServer��˽�г�Ա����
    HANDLE m_handle;
    bool m_bStop;

    //����ͻ��ˣ��߳�id����socket�Ķ�Ӧ��ϵ
    map<unsigned int,SOCKET>m_mapThreadIdToSocket;

    //�����߳̾��
    list<HANDLE>m_listHandle;

};
