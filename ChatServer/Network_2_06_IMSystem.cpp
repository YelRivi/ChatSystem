#include<iostream>
#include"net/INet.h"
#include"net/UdpNet.h"
#include"Ckernel.h"

using namespace std;

int main() {

	//�򿪷����
	Ckernel kernel;
	if (!kernel.startServer()) {
		cout << "�򿪷�����ʧ��" << endl;
		kernel.closeServer();
		return 1;
	}
	
	//�ó���һֱ����
	while (true) {
		Sleep(5000);
		cout << "server is running" << endl;
	}
	return 0;
}