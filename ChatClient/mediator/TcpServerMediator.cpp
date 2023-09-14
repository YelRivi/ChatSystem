#include"TcpServerMediator.h"
#include"./net/TcpServer.h"

TcpServerMediator::TcpServerMediator() {

	m_pNet = new TcpServer(this);
}
TcpServerMediator::~TcpServerMediator() {
	if (m_pNet) {
		delete m_pNet;
		m_pNet = NULL;
	}

}
//打开网络
bool TcpServerMediator::OpenNet() {
	return m_pNet->InitNet();
}

//发送数据
bool TcpServerMediator::SendData(char* sendData, int nLen, long lSend) {
	return m_pNet->SendData(sendData, nLen, lSend);
}

//处理数据
void TcpServerMediator::DealData(char* recvData, int nLen, long lFrom) {
    cout << "TcpServerMediator::DealData " << recvData;
	SendData("TcpServerMediator::DealData", sizeof("TcpServerMediator::DealData"),lFrom);
}

//关闭网络
void TcpServerMediator::CloseNet() {
	m_pNet->UnInitData();
}
