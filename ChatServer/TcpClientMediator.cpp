#include"TcpClientMediator.h"
#include"TcpClient.h"

TcpClientMediator::TcpClientMediator() {

	m_pNet = new TcpClient(this);
}
TcpClientMediator::~TcpClientMediator() {


}
//������
bool TcpClientMediator::OpenNet() {
	return m_pNet->InitNet();
}

//��������
bool TcpClientMediator::SendData(char* sendData, int nLen, long lSend) {
	return m_pNet->SendData(sendData, nLen, lSend);
}

//��������
void TcpClientMediator::DealData(char* recvData, int nLen, long lFrom) {
	cout << "TcpClientMediator::DealData" << recvData << endl;
}

//�ر�����
void TcpClientMediator::CloseNet() {
	m_pNet->UnInitData();
}