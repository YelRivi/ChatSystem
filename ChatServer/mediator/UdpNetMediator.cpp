#include"UdpNetMediator.h"
#include"../net/UdpNet.h"

UdpNetMediator::UdpNetMediator() {

	m_pNet = new UdpNet(this);
}
UdpNetMediator::~UdpNetMediator() {


}
//������
bool UdpNetMediator::OpenNet() {
	return m_pNet->InitNet();
}

//��������
bool UdpNetMediator::SendData(char* sendData, int nLen, long lSend) {
	return m_pNet->SendData(sendData, nLen, lSend);
}

//��������
void UdpNetMediator::DealData(char* recvData, int nLen, long lFrom) {
	cout << recvData << endl;
}

//�ر�����
void UdpNetMediator::CloseNet() {
	m_pNet->UnInitData();
}