#pragma once
#include"INetMediator.h"

class TcpClientMediator :public INetMediator {
public:
	TcpClientMediator();
	~TcpClientMediator();
	//������
	bool OpenNet();

	//��������
	bool SendData(char* sendData, int nLen, long lSend);

	//��������
	void DealData(char* recvData, int nLen, long lFrom);

	//�ر�����
	void CloseNet();
protected:

};