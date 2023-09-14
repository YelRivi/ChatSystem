#include"UdpNetMediator.h"
#include"../net/UdpNet.h"

UdpNetMediator::UdpNetMediator() {

	m_pNet = new UdpNet(this);
}
UdpNetMediator::~UdpNetMediator() {


}
//打开网络
bool UdpNetMediator::OpenNet() {
	return m_pNet->InitNet();
}

//发送数据
bool UdpNetMediator::SendData(char* sendData, int nLen, long lSend) {
	return m_pNet->SendData(sendData, nLen, lSend);
}

//处理数据
void UdpNetMediator::DealData(char* recvData, int nLen, long lFrom) {
	cout << recvData << endl;
}

//关闭网络
void UdpNetMediator::CloseNet() {
	m_pNet->UnInitData();
}