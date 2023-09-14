#pragma once
#include"../net/INet.h"

class INet;
class INetMediator {
public:
	INetMediator(){}
	virtual ~INetMediator(){}
	//打开网络
	virtual bool OpenNet() = 0;

	//发送数据
	virtual bool SendData(char* sendData, int nLen, long lSend) = 0;

	//处理数据
	virtual void DealData(char* recvData, int nLen, long lFrom) = 0;

	//关闭网络
	virtual void CloseNet() = 0;
protected:
	INet* m_pNet;

};