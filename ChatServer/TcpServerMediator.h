#pragma once
#include"INetMediator.h"

class TcpServerMediator :public INetMediator {
public:
	TcpServerMediator();
	~TcpServerMediator();
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