#include"INetMediator.h"

class UdpNetMediator:public INetMediator{
public:
	UdpNetMediator();
	~UdpNetMediator();
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