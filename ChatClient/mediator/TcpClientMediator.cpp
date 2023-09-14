#include"TcpClientMediator.h"
#include"./net/TcpClient.h"

TcpClientMediator::TcpClientMediator() {

	m_pNet = new TcpClient(this);
}
TcpClientMediator::~TcpClientMediator() {

    if(m_pNet){
        delete m_pNet;
        m_pNet = NULL;
    }

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
    //把接收到的数据发给kernel
    Q_EMIT SIG_ReadyData(recvData,nLen,lFrom);
}

//关闭网络
void TcpClientMediator::CloseNet() {
	m_pNet->UnInitData();
}
