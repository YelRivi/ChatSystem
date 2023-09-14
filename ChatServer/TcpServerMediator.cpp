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
//������
bool TcpServerMediator::OpenNet() {
	return m_pNet->InitNet();
}

//��������
bool TcpServerMediator::SendData(char* sendData, int nLen, long lSend) {
	return m_pNet->SendData(sendData, nLen, lSend);
}

//��������
void TcpServerMediator::DealData(char* recvData, int nLen, long lFrom) {

	//�ѽ��յ������ݷ���kernel
	Ckernel::pKernel->dealReadyData(recvData, nLen, lFrom);
}

//�ر�����
void TcpServerMediator::CloseNet() {
	m_pNet->UnInitData();
}