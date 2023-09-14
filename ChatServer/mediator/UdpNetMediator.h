#include"INetMediator.h"

class UdpNetMediator:public INetMediator{
public:
	UdpNetMediator();
	~UdpNetMediator();
	//打开网络
	bool OpenNet();

	//发送数据
	bool SendData(char* sendData, int nLen, long lSend);

	//处理数据
	void DealData(char* recvData, int nLen, long lFrom);

	//关闭网络
	void CloseNet();
protected:

};