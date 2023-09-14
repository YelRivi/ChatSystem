#pragma once
#include"../net/INet.h"

class INet;
class INetMediator {
public:
	INetMediator(){}
	virtual ~INetMediator(){}
	//������
	virtual bool OpenNet() = 0;

	//��������
	virtual bool SendData(char* sendData, int nLen, long lSend) = 0;

	//��������
	virtual void DealData(char* recvData, int nLen, long lFrom) = 0;

	//�ر�����
	virtual void CloseNet() = 0;
protected:
	INet* m_pNet;

};