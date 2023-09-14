#include"TcpServerMediator.h"
#include"TcpServer.h"
#include"Ckernel.h"

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

	//把接收到的数据发给kernel
	Ckernel::pKernel->dealReadyData(recvData, nLen, lFrom);
}

//关闭网络
void TcpServerMediator::CloseNet() {
	m_pNet->UnInitData();
}