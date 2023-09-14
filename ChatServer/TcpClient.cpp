#include"TcpClient.h"
#include"TcpClientMediator.h"

TcpClient::TcpClient(INetMediator* pMediator):m_sock(INVALID_SOCKET),m_bStop(false),m_handle(NULL) {
	m_pMediator = pMediator;

}

	TcpClient::~TcpClient() {

}
//��ʼ������
	bool TcpClient::InitNet() {
		//1.���ؿ�
		WORD wVersion = MAKEWORD(2, 2);
		WSADATA data;
		int err = WSAStartup(wVersion, &data);

		if (0 != err) {
			cout << "WSAStartup fail." << endl;
			return false;
		}

		if (2 != HIBYTE(data.wVersion) || 2 != LOBYTE(data.wVersion)) {
			cout << "WSAStartup version error." << endl;
			return false;
		}
		else {
			cout << "WSAStartup success." << endl;
		}
		//2�������׽���
		m_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (INVALID_SOCKET == m_sock) {		//�׽��ִ������
			cout << "Socket error:" << WSAGetLastError() << endl;
			return false;
		}
		else {
			cout << "Socket success." << endl;
		}
		//3�����ӷ����
		sockaddr_in addrServer;
		addrServer.sin_family = AF_INET;
		addrServer.sin_port = htons(_DEF_TCP_PORT);
		addrServer.sin_addr.S_un.S_addr = inet_addr(_DEF_TCP_SERVER_IP);
		err = connect(m_sock, (sockaddr*)&addrServer, sizeof(addrServer));
		if (SOCKET_ERROR == err) {
			cout << "Bind error:" << WSAGetLastError() << endl;
		}
		else {
			cout << "Bind success." << endl;

		}

		//4�������������ݵ��߳�
		m_handle = (HANDLE)_beginthreadex(0, 0, &RecvThread, this, 0, NULL);

		return true;
}
//�������ݵ��̺߳���
unsigned __stdcall TcpClient::RecvThread(void* lpVoid) {

	TcpClient* pThis = (TcpClient*)lpVoid;
	pThis->RecvData();

	return 1;
}

//��������(UDP��IP��ַ��TPC��socket)
	bool TcpClient::SendData(char* sendData, int nLen, long lSend) {
		//У������Ϸ���
		if (!sendData || nLen <= 0) {
			cout << "Parameter error:" << WSAGetLastError() << endl;
			return false;
		}

		//�ȷ�����С
		if (send(m_sock, (char*)&nLen, sizeof(int), 0) <= 0) {
			cout << "Send error:" << WSAGetLastError() << endl;
			return false;
		}

		//�ٷ�������
		if (send(m_sock, sendData, nLen, 0) <= 0) {
			cout << "Send error:" << WSAGetLastError() << endl;
			return false;
		}
		
		return true;

}
//�ر�����:�����߳���Դ�������̺߳������رվ�������ر��׽��֡�ж�ؿ�
	void TcpClient::UnInitData() {
		//1�������߳���Դ
		m_bStop = true;
		if (m_handle) {
			if (WAIT_TIMEOUT == WaitForSingleObject(m_handle, 500)) {
				TerminateThread(m_handle,-1);
			}
			CloseHandle(m_handle);
				m_handle = NULL;
		}
		//2���ر��׽���
		if (!m_sock && INVALID_SOCKET != m_sock) {
			closesocket(m_sock);
		}
		//3��ж�ؿ�
		WSACleanup();

}

   //��������
	void TcpClient::RecvData() {

		int nRecvNum = 0;
		int nPackSize = 0;
		int nOffset = 0;
		while (!m_bStop) {
			//�Ƚ��հ���С
			nRecvNum = recv(m_sock, (char*)&nPackSize, sizeof(int), 0);
			if (nRecvNum > 0) {
				//���ճɹ���newһ���ռ��������
				char* packBuf = new char[nPackSize];

				//���հ�����
				while (nPackSize > 0) {
					nRecvNum = recv(m_sock, packBuf + nOffset, nPackSize, 0);
					if (nRecvNum > 0) {
						nPackSize -= nRecvNum;
						nOffset += nRecvNum;
					}
					else {
						cout << "Recv error:" << WSAGetLastError() << endl;
						break;
					}
				}
				//����һ������������ɣ������ݷ����н���
				m_pMediator->DealData(packBuf, nOffset, m_sock);
				//ƫ��������
				nOffset = 0;
			}
			else {
				cout << "Recv error:" << WSAGetLastError() << endl;
				break;
			}
		}
 }
 