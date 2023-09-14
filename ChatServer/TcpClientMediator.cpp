#include"TcpClientMediator.h"
#include"TcpClient.h"

TcpClientMediator::TcpClientMediator() {

	m_pNet = new TcpClient(this);
}
TcpClientMediator::~TcpClientMediator() {


}
//打开网络
bool TcpClientMediator::OpenNet() {
	return m_pNet->InitNet();
}

//发送数据
bool TcpClientMediator::SendData(char* sendData, int nLen, long lSend) {
	return m_pNet->SendData(sendData, nLen, lSend);
}

//处理数据
void TcpClientMediator::DealData(char* recvData, int nLen, long lFrom) {
	cout << "TcpClientMediator::DealData" << recvData << endl;
}

//关闭网络
void TcpClientMediator::CloseNet() {
	m_pNet->UnInitData();
}