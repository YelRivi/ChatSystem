#include"TcpClient.h"
#include"./mediator/TcpClientMediator.h"

TcpClient::TcpClient(INetMediator* pMediator):m_sock(INVALID_SOCKET),m_bStop(false),m_handle(NULL) {
	m_pMediator = pMediator;

}

	TcpClient::~TcpClient() {

}
//初始化网络
	bool TcpClient::InitNet() {
		//1.加载库
		WORD wVersion = MAKEWORD(2, 2);
		WSADATA data;
		int err = WSAStartup(wVersion, &data);

		if (0 != err) {
			cout << "WSAStartup fail." << endl;
			return false;
		}

		if (2 != HIBYTE(data.wHighVersion) || 2 != LOBYTE(data.wVersion)) {
			cout << "WSAStartup version error." << endl;
			return false;
		}
		else {
			cout << "WSAStartup success." << endl;
		}
		//2、创建套接字
		m_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (INVALID_SOCKET == m_sock) {		//套接字错误代码
			cout << "Socket error:" << WSAGetLastError() << endl;
			return false;
		}
		else {
			cout << "Socket success." << endl;
		}
		//3、连接服务端
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

		//4、创建接收数据的线程
		m_handle = (HANDLE)_beginthreadex(0, 0, &RecvThread, this, 0, NULL);

		return true;
}
//接收数据的线程函数
unsigned __stdcall TcpClient::RecvThread(void* lpVoid) {

	TcpClient* pThis = (TcpClient*)lpVoid;
	pThis->RecvData();

	return 1;
}

//发送数据(UDP是IP地址，TCP是socket)
	bool TcpClient::SendData(char* sendData, int nLen, long lSend) {
		//校验参数合法性
		if (!sendData || nLen <= 0) {
			cout << "Parameter error:" << WSAGetLastError() << endl;
			return false;
		}

		//先发包大小
		if (send(m_sock, (char*)&nLen, sizeof(int), 0) <= 0) {
			cout << "Send error:" << WSAGetLastError() << endl;
			return false;
		}

		//再发包内容
		if (send(m_sock, sendData, nLen, 0) <= 0) {
			cout << "Send error:" << WSAGetLastError() << endl;
			return false;
		}
		
		return true;

}
//关闭网络:回收线程资源（结束线程函数、关闭句柄）、关闭套接字、卸载库
	void TcpClient::UnInitData() {
		//1、回收线程资源
		m_bStop = true;
		if (m_handle) {
			if (WAIT_TIMEOUT == WaitForSingleObject(m_handle, 500)) {
				TerminateThread(m_handle,-1);
			}
			CloseHandle(m_handle);
				m_handle = NULL;
		}
		//2、关闭套接字
		if (!m_sock && INVALID_SOCKET != m_sock) {
			closesocket(m_sock);
		}
		//3、卸载库
		WSACleanup();

}

   //接收数据
	void TcpClient::RecvData() {

		int nRecvNum = 0;
		int nPackSize = 0;
		int nOffset = 0;
		while (!m_bStop) {
			//先接收包大小
			nRecvNum = recv(m_sock, (char*)&nPackSize, sizeof(int), 0);
			if (nRecvNum > 0) {
				//接收成功，new一个空间接受数据
				char* packBuf = new char[nPackSize];

				//接收包内容
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
				//接收一个包的数据完成，把数据发给中介者
				m_pMediator->DealData(packBuf, nOffset, m_sock);
				//偏移量清零
				nOffset = 0;
			}
			else {
				cout << "Recv error:" << WSAGetLastError() << endl;
				break;
			}
		}
 }
 
