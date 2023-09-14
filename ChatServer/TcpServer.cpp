#include"TcpServer.h"
#include"net/packDef.h"
#include<process.h>
#include"TcpServerMediator.h"

TcpServer::TcpServer(INetMediator* pMediator):m_sock(INVALID_SOCKET),m_bStop(false),m_handle(NULL) {
	m_pMediator = pMediator;

}
TcpServer::~TcpServer() {

}
//��ʼ������:���ؿ⡢�����׽��֡���ip�������������������ӵ��߳�
bool TcpServer::InitNet() {

    //1.���ؿ�
	WORD wVersion = MAKEWORD(2, 2);
	WSADATA data;
	int err = WSAStartup(wVersion,&data);

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
	//3����ip
	sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(_DEF_TCP_PORT);
	addr.sin_addr.S_un.S_addr = INADDR_ANY;
	err = bind(m_sock, (sockaddr*)&addr, sizeof(addr));
	if (SOCKET_ERROR == err) {
		cout << "Bind error:" << WSAGetLastError() << endl;
	}
	else {
		cout << "Bind success."<< endl;

	}

	//4������
	err = listen(m_sock, 10);
	if (SOCKET_ERROR == err) {
		cout << "Listen error:" << WSAGetLastError() << endl;
	}
	else {
		cout << "Listen success." << endl;

	}


	//5�������������ӵ��߳�
	HANDLE handle = (HANDLE)_beginthreadex(0, 0, &AcceptThread, this, 0, NULL);
	if (handle) {
		m_listHandle.push_back(handle);
	}
	return true;

}

//�������ӵ��̺߳���
unsigned __stdcall TcpServer::AcceptThread(void* lpVoid) {

	//�´���һ�������ָ��
	TcpServer* pThis = (TcpServer*)lpVoid;
	sockaddr_in addrClient;
	int addrClientSize = sizeof(addrClient);
	SOCKET sock = INVALID_SOCKET;
	unsigned int threadId = 0;

	while (!pThis->m_bStop) {
		sock = accept(pThis->m_sock,(sockaddr*)&addrClient, &addrClientSize);
		if (INVALID_SOCKET != sock) {
			cout << "Client ip:" << inet_ntoa(addrClient.sin_addr) << endl;
		}
		else {
			cout << "Accept error:" << WSAGetLastError() << endl;
			continue;
		}

		//�����������ݵ��߳�
		HANDLE handle = (HANDLE)_beginthreadex(0, 0, &RecvThread, pThis, 0, &threadId);
		if (handle) {
			pThis->m_listHandle.push_back(handle);
		}
		//����ͻ��˵�socket
		pThis->m_mapThreadIdToSocket[threadId] = sock;
	}
	return 1;
}

//�������ݵ��̺߳���
unsigned __stdcall TcpServer::RecvThread(void* lpVoid) {
	TcpServer* pThis = (TcpServer*)lpVoid;
	pThis->RecvData();
	return 1;
 }


//��������(UDP��IP��ַ��TCP��socket)
bool TcpServer::SendData(char* sendData, int nLen, long lSend) {

	//У������Ϸ���
	if (!sendData || nLen <= 0) {
		cout << "Socket error." << WSAGetLastError() << endl;
		return false;
	}

	//�ȷ�����С
	if (send(lSend,(char*)&nLen,sizeof(int),0) <= 0) {
		cout << "Send error:" << WSAGetLastError() << endl;
		return false;
	}

	//�ٷ�������
	if (send(lSend,sendData, nLen,0) <= 0) {
		cout << "Send error:" << WSAGetLastError() << endl;
		return false;
	}

	return true;

}

//�ر�����:�����߳���Դ�������̺߳������رվ�������ر��׽��֡�ж�ؿ�
void TcpServer::UnInitData() {
	//1�������߳���Դ
	m_bStop = true;
	for (auto ite = m_listHandle.begin(); ite != m_listHandle.end();) {
		HANDLE handle = *ite;
		if (handle) {
			//�ȴ��߳��Զ�����ѭ��
			if (WAIT_TIMEOUT == WaitForSingleObject(handle,500)) {
				TerminateThread(handle, -1);
			}
			//�رվ��
			CloseHandle(handle);
			handle = NULL;
		}
		//����Ч����map���Ƴ�
		ite = m_listHandle.erase(ite);
	}
	//2���ر��׽���
	if (!m_sock && INVALID_SOCKET != m_sock) {
		closesocket(m_sock);
	}
	for (auto ite = m_mapThreadIdToSocket.begin(); ite != m_mapThreadIdToSocket.end();) {
		SOCKET sock = ite->second;
		if (!sock && INVALID_SOCKET != sock) {
			closesocket(sock);
		}
		//����Ч�ڵ��map���Ƴ�
		ite = m_mapThreadIdToSocket.erase(ite);
	}

	//3��ж�ؿ�
	WSACleanup();
}
//��������
void TcpServer::RecvData() {

	//���ߣ��ȴ�socket����map��֮����ȡ
	Sleep(100);		//����cpuʹ��ʱ��

	//��ȡ��ǰ�̶߳�Ӧ��socket
	SOCKET sock = m_mapThreadIdToSocket[GetCurrentThreadId()];
	if (!sock || INVALID_SOCKET == sock) {
		cout << "Socket error." << WSAGetLastError() << endl;
		return;
	}

	int nRecvNum = 0;
	int nPackSize = 0;
	int nOffset = 0;
	while (!m_bStop) {
		//�Ƚ��հ���С
		nRecvNum = recv(sock,(char*)&nPackSize,sizeof(int),0);
		if (nRecvNum > 0) {
			//���ճɹ���newһ���ռ��������
			char* packBuf = new char[nPackSize];

			//���հ�����
			//�ӻ��������գ�ÿһ�ν���һ���������ݣ�����֮�����´���
			//������Ĵ�С�Դ���0
			while (nPackSize > 0) {
				//ͨ��ƫ����ƴ��ÿ������
				nRecvNum = recv(sock, packBuf + nOffset, nPackSize, 0);
				//�����������������
				if (nRecvNum > 0) {
					//��ȥ��һ�ν��յ����ݳ���
					nPackSize -= nRecvNum;
					//ƫ�������ӵ���һ�ν��յ�ͷ��
					nOffset += nRecvNum;
				}
				else {
					cout << "Recv error:" << WSAGetLastError() << endl;
					break;
				}
			}
			//����һ������������ɣ������ݷ����н���
			m_pMediator->DealData(packBuf, nOffset, sock);
			//ƫ��������
			nOffset = 0;
		}
		else {
			cout << "Recv error:" << WSAGetLastError() << endl;
			break;
		}
	}
}