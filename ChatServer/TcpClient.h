#pragma once
#include"./net/INet.h"
#include<process.h>

class TcpClient :public INet {
public:
    TcpClient(INetMediator* pMediator);
    ~TcpClient();
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

    SOCKET m_sock;  
    HANDLE m_handle;
    bool m_bStop;



};
