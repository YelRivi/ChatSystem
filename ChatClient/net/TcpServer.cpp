#include"TcpServer.h"
#include"net/packDef.h"
#include<process.h>
#include"./mediator/TcpServerMediator.h"

TcpServer::TcpServer(INetMediator* pMediator):m_sock(INVALID_SOCKET),m_bStop(false),m_handle(NULL) {
	m_pMediator = pMediator;

}
TcpServer::~TcpServer() {

}
//初始化网络:加载库、创建套接字、绑定ip、监听、创建接受连接的线程
bool TcpServer::InitNet() {

    //1.加载库
	WORD wVersion = MAKEWORD(2, 2);
	WSADATA data;
	int err = WSAStartup(wVersion,&data);

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
	//3、绑定ip
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

	//4、监听
	err = listen(m_sock, 10);
	if (SOCKET_ERROR == err) {
		cout << "Listen error:" << WSAGetLastError() << endl;
	}
	else {
		cout << "Listen success." << endl;

	}


	//5、创建接受连接的线程
	HANDLE handle = (HANDLE)_beginthreadex(0, 0, &AcceptThread, this, 0, NULL);
	if (handle) {
		m_listHandle.push_back(handle);
	}
	return true;

}

//接受连接的线程函数
unsigned __stdcall TcpServer::AcceptThread(void* lpVoid) {

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

		//创建接收数据的线程
		HANDLE handle = (HANDLE)_beginthreadex(0, 0, &RecvThread, pThis, 0, &threadId);
		if (handle) {
			pThis->m_listHandle.push_back(handle);
		}
		//保存客户端的socket
		pThis->m_mapThreadIdToSocket[threadId] = sock;
	}
	return 1;
}

//接收数据的线程函数
unsigned __stdcall TcpServer::RecvThread(void* lpVoid) {
	TcpServer* pThis = (TcpServer*)lpVoid;
	pThis->RecvData();
	return 1;
 }


//发送数据(UDP是IP地址，TPC是socket)
bool TcpServer::SendData(char* sendData, int nLen, long lSend) {

	//校验参数合法性
	if (!sendData || nLen <= 0) {
		cout << "Socket error." << WSAGetLastError() << endl;
		return false;
	}

	//先发包大小
	if (send(lSend,(char*)&nLen,sizeof(int),0) <= 0) {
		cout << "Send error:" << WSAGetLastError() << endl;
		return false;
	}

	//再发包内容
	if (send(lSend,sendData, nLen,0) <= 0) {
		cout << "Send error:" << WSAGetLastError() << endl;
		return false;
	}

	return true;

}
//关闭网络:回收线程资源（结束线程函数、关闭句柄）、关闭套接字、卸载库
void TcpServer::UnInitData() {
	//1、回收线程资源
	m_bStop = true;
	for (auto ite = m_listHandle.begin(); ite != m_listHandle.end();) {
		HANDLE handle = *ite;
		if (handle) {
			//等待线程自动结束循环
			if (WAIT_TIMEOUT == WaitForSingleObject(handle,500)) {
				TerminateThread(handle, -1);
			}
			//关闭句柄
			CloseHandle(handle);
			handle = NULL;
		}
		//把无效结点从map中移除
		ite = m_listHandle.erase(ite);
	}
	//2、关闭套接字
	if (!m_sock && INVALID_SOCKET != m_sock) {
		closesocket(m_sock);
	}
	for (auto ite = m_mapThreadIdToSocket.begin(); ite != m_mapThreadIdToSocket.end();) {
		SOCKET sock = ite->second;
		if (!sock && INVALID_SOCKET != sock) {
			closesocket(sock);
		}
		//把无效节点从map中移除
		ite = m_mapThreadIdToSocket.erase(ite);
	}

	//3、卸载库
	WSACleanup();
}
//接收数据
void TcpServer::RecvData() {

	//休眠，等待socket存入map中之后再取
	Sleep(100);		//放弃cpu使用时间

	//获取当前线程对应的socket
	SOCKET sock = m_mapThreadIdToSocket[GetCurrentThreadId()];
	if (!sock || INVALID_SOCKET == sock) {
		cout << "Socket error." << WSAGetLastError() << endl;
		return;
	}

	int nRecvNum = 0;
	int nPackSize = 0;
	int nOffset = 0;
	while (!m_bStop) {
		//先接收包大小
		nRecvNum = recv(sock,(char*)&nPackSize,sizeof(int),0);
		if (nRecvNum > 0) {
			//接收成功，new一个空间接受数据
			char* packBuf = new char[nPackSize];

			//接收包内容
			while (nPackSize > 0) {
				nRecvNum = recv(sock, packBuf + nOffset, nPackSize, 0);
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
			m_pMediator->DealData(packBuf, nOffset, sock);
			//偏移量清零
			nOffset = 0;
		}
		else {
			cout << "Recv error:" << WSAGetLastError() << endl;
			break;
		}
	}
}
