#include<iostream>
#include"net/INet.h"
#include"net/UdpNet.h"
#include"Ckernel.h"

using namespace std;

int main() {

	//打开服务端
	Ckernel kernel;
	if (!kernel.startServer()) {
		cout << "打开服务器失败" << endl;
		kernel.closeServer();
		return 1;
	}
	
	//让程序一直运行
	while (true) {
		Sleep(5000);
		cout << "server is running" << endl;
	}
	return 0;
}