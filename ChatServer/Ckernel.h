#pragma once
#include<iostream>
#include"mediator/INetMediator.h"
#include"CMySql.h"
#include<map>

using namespace std;
//���庯��ָ��
class Ckernel;
typedef void(Ckernel::* pFun)(char* recvData, int nLen, long lFrom);

class Ckernel
{
public:
	Ckernel();
	~Ckernel();


	//�򿪷�����
	bool startServer();;
	//�رշ�����
	void closeServer();

	//��ʼ��Э��ͷ����
	void setProtocolMap();

	//��ȡ��ǰ��½�û��ĺ�����Ϣ���Լ�����Ϣ
	void getFriendInfo(int userId);

	//�����û�id��ѯ�û���Ϣ
	void getUserInfo(int id,_STRU_TCP_FRIEND_INFO* info);

	//�������н��յ�������
	void dealReadyData(char* recvData, int nLen, long lFrom);

	//����ע������
	void dealRegisterRq(char* recvData, int nLen, long lFrom);

	//�����¼����
	void dealLoginRq(char* recvData, int nLen, long lFrom);

	//������������
	void dealOfflineRq(char* recvData, int nLen, long lFrom);

	//������������
	void dealChatRq(char* recvData, int nLen, long lFrom);

	//������Ӻ�������
	void dealAddFriendRq(char* recvData, int nLen, long lFrom);

	//������Ӻ��ѻظ�
	void dealAddFriendRs(char* recvData, int nLen, long lFrom);


	static Ckernel* pKernel;
private:
	//�н�ָ��
	INetMediator* m_pMediator;

	//�������ݿ�Ķ���
	CMySql m_sql;

	//���庯��ָ������
	pFun m_mapPackTypeToFun [_DEF_PROTOCOL_COUNT];

	//���浱ǰ��½�ɹ����û�id��socket
	//���ã�1�����ͻ��˷���Ϣ��ʱ�����ȡ���ͻ��˵�socket;2���жϿͻ����Ƿ����ߣ�
	map<int, SOCKET> m_mapIdToSocket;

};

