#include "Ckernel.h"
#include"TcpServerMediator.h"
#include"packDef.h"

//��̬��Ա���������ʼ��
Ckernel* Ckernel::pKernel = nullptr;

Ckernel::Ckernel()
{
	//��ʼ��Э��ͷ����
	setProtocolMap();
	cout << __func__ << endl;
	pKernel = this;
}

Ckernel::~Ckernel()
{
	cout << __func__ << endl;

}

bool Ckernel::startServer()
{
	cout << __func__ << endl;
	m_pMediator = new TcpServerMediator;
	//������
	if (!m_pMediator->OpenNet()) {
		cout << "������ʧ��" << endl;
		return false;
	}

	//�������ݿ�
	if (!m_sql.ConnectMySql("127.0.0.1","root","schoolidol","improject")) {
		cout << "�������ݿ�ʧ��" << endl;
		return false;
	}
}

//�رշ�����
void Ckernel::closeServer()
{
	cout << __func__ << endl;
	if (m_pMediator) {
		m_pMediator->CloseNet();
		delete m_pMediator;
		m_pMediator = nullptr;
	}

	//�Ͽ������ݿ������
	m_sql.DisConnect();
}

//��Э��ͷ�ʹ����� 
void Ckernel::setProtocolMap()
{
	cout << __func__ << endl;
	//�����ʼ����0
	memset(m_mapPackTypeToFun, 0, sizeof(m_mapPackTypeToFun));

	//��Э��ͷ�ʹ�����
	m_mapPackTypeToFun[_DEF_TCP_REGISTER_RQ - _DEF_PROTOCOL_BASE - 1 ]  = &Ckernel::dealRegisterRq;
	m_mapPackTypeToFun[_DEF_TCP_LOGIN_RQ - _DEF_PROTOCOL_BASE - 1]	    = &Ckernel::dealLoginRq;
	m_mapPackTypeToFun[_DEF_TCP_OFFLINE_RQ - _DEF_PROTOCOL_BASE - 1]    = &Ckernel::dealOfflineRq;
	m_mapPackTypeToFun[_DEF_TCP_CHAT_RQ       - _DEF_PROTOCOL_BASE - 1] = &Ckernel::dealChatRq;
	m_mapPackTypeToFun[_DEF_TCP_ADD_FRIEND_RQ - _DEF_PROTOCOL_BASE - 1] = &Ckernel::dealAddFriendRq;
	m_mapPackTypeToFun[_DEF_TCP_ADD_FRIEND_RS - _DEF_PROTOCOL_BASE - 1] = &Ckernel::dealAddFriendRs;

}

//�������н��յ�������
void Ckernel::dealReadyData(char* recvData, int nLen, long lFrom)
{

	cout << __func__ << endl;
	//ȡ��Э��ͷ
	PackType type = *(PackType*)recvData;//Ϊʲô��

	//����Э��ͷ�ߵ���ͬ�Ĵ�������
	//���������±�
	int index = type - _DEF_PROTOCOL_BASE - 1;
	//У�������±��Ƿ��ںϷ���Χ��
	if (0 <= index && index < _DEF_PROTOCOL_COUNT) {
		pFun pf = m_mapPackTypeToFun[index];
		if (pf) {
			(this->*pf)(recvData, nLen, lFrom);
		}
		else {
			cout << "invalid type:" << type;
		}
	}
	else {
		cout << "invalid type:" << type;
	}

	//ɾ��������Ŀռ�
	delete[] recvData;

}

//����ע������
void Ckernel::dealRegisterRq(char* recvData, int nLen, long lFrom) {

	cout << __func__ << endl;
	//���
	_STRU_TCP_REGISTER_RQ* rq = (_STRU_TCP_REGISTER_RQ*)recvData;
	
	//У�����ݺϷ��ԣ���ͻ���У�����һ�£�
	// 
	//���ݵ绰��ѯ���ݿ�
	list<string> listRes;
	char sqlBuf[1024] = "";			//�洢sql���
	sprintf(sqlBuf,"select tel from t_user where tel = '%s'",rq->tel);						//sprintf:���ַ�����ӡ��ָ���ռ�
	if (!m_sql.SelectMySql(sqlBuf,1 ,listRes)) {
		cout << "��ѯ���ݿ�ʧ�ܣ�" << sqlBuf << endl;
		return;
	}
	//�жϲ�ѯ���
	_STRU_TCP_REGISTER_RS rs;
	if (listRes.size() > 0) {
		//ע��ʧ�ܣ��绰�����ѱ�ע��
		rs.result = register_tel_used;
	}
	else {
		//�����ǳƲ�ѯ���ݿ�
		sprintf(sqlBuf, "select name from t_user where name = '%s'", rq->name);						//sprintf:���ַ�����ӡ��ָ���ռ�
		if (!m_sql.SelectMySql(sqlBuf, 1, listRes)) {
			cout << "��ѯ���ݿ�ʧ�ܣ�"<< sqlBuf << endl;
			return;
		}
		//�жϲ�ѯ���
		if (listRes.size() > 0) {
			//ע��ʧ�ܣ��ǳ��ѱ�ע��
			rs.result = register_name_used;
		}
		else {
			//���û���Ϣд�����ݿ�
			sprintf(sqlBuf, "insert into t_user (name, tel, password, feeling, iconid) values ('%s', '%s', '%s', '��ã�',13);",rq->name,rq->tel,rq->password);
			if (!m_sql.UpdateMySql(sqlBuf)) {
				cout << "�������ݿ�ʧ�ܣ�" << sqlBuf << endl;
				return;
			}

			//ע��ɹ�
			rs.result = register_success;
		}
	}

	//���ͻ��˷���ע����
	m_pMediator->SendData((char*)&rs,sizeof(rs),lFrom);

}

//��¼����
void Ckernel::dealLoginRq(char* recvData, int nLen, long lFrom)
{
	cout << __func__ << endl;
	//���
	_STRU_TCP_LOGIN_RQ* rq = (_STRU_TCP_LOGIN_RQ*)recvData;

	//У�����ݺϷ��ԣ���ͻ���У�����һ�£�
	
	//���ݵ绰��id������
	list<string> listRes;
	char sqlBuf[1024] = "";			//�洢sql���
	sprintf(sqlBuf, "select id, password from t_user where tel = '%s';", rq->tel);						//sprintf:���ַ�����ӡ��ָ���ռ�
	if (!m_sql.SelectMySql(sqlBuf, 2, listRes)) {
		cout << "��ѯ���ݿ�ʧ�ܣ�" << sqlBuf << endl;
		return;
	}
	//�жϲ�ѯ���
	_STRU_TCP_LOGIN_RS rs;
	//�жϲ��ҽ��
	if (0 == listRes.size()) {
		//û�в鵽����¼ʧ��,login_no_tel
		rs.result = login_no_tel;
	}
	else {
		//ȡ��id
		int id = stoi(listRes.front());
		listRes.pop_front();				//ÿ�ζ��Ǵ�listRes��ͷ��ʼȡ��ȡ��֮��Ҫɾ��

		//ȡ������
		string password = listRes.front();			//ȡ����һ����ĸ
		listRes.pop_front();						//������һ����ĸ

		//�Ƚ������Ƿ���ͬ
		//ʹ��strcmp�Ƚ��ַ���
		if (0 == strcmp(password.c_str(),rq->password)) {	//password��Ҫת���ɱ�׼char����
			//��ͬ����¼�ɹ�	login_success
			rs.result = login_success;
			rs.userId = id;

			//��¼��������ͻ���
			m_pMediator->SendData((char*)&rs, sizeof(rs), lFrom);

			//���浱ǰ��¼�û���socket��id
			m_mapIdToSocket[id] = lFrom;

			// ��ȡ����id�б�
			// �ж�ÿ�������Ƿ����ߣ����߾�֪ͨ

			//��ȡ��ǰ��¼�û��ĺ�����Ϣ���Լ�����Ϣ
			getFriendInfo(id);

			return;
		}
		else {
			//��ͬ����¼ʧ��	login_password_error
			rs.result = login_password_error;
		}
	}

	//��¼��������ͻ���
	m_pMediator->SendData((char*)&rs, sizeof(rs), lFrom);
}

//������������
void Ckernel::dealOfflineRq(char* recvData, int nLen, long lFrom)
{

	cout << __func__ << endl;
	//���
	_STRU_TCP_OFFLINE_RQ* rq = (_STRU_TCP_OFFLINE_RQ*)recvData;

	//�����û�id��ѯ����id�б�
	list<string> listRes;
	char sqlBuf[1024] = "";			//�洢sql���
	sprintf(sqlBuf, "select idB from t_friend where idA = '%d';", rq->userId);						//sprintf:���ַ�����ӡ��ָ���ռ�
	if (!m_sql.SelectMySql(sqlBuf, 1, listRes)) {
		cout << "��ѯ���ݿ�ʧ�ܣ�" << sqlBuf << endl;
		return;
	}

	//��������id�б�
	int friendId = 0;
	_STRU_TCP_FRIEND_INFO friendInfo;
	while (listRes.size() > 0) {

		//ȡ������id
		friendId = stoi(listRes.front());
		listRes.pop_front();

		//�жϺ����Ƿ�����
		if (m_mapIdToSocket.count(friendId) > 0) {
			//����������ߣ�ת����������
			m_pMediator->SendData(recvData,nLen, m_mapIdToSocket[friendId]);
		}
	}
	//���Լ���map���Ƴ�
	if (m_mapIdToSocket.count(rq->userId) > 0) {
		closesocket(m_mapIdToSocket[rq->userId]);
		m_mapIdToSocket.erase(rq->userId);
	}
}

//������������
void Ckernel::dealChatRq(char* recvData, int nLen, long lFrom)
{
	cout << __func__ << endl;
	//���
	_STRU_TCP_CHAT_RQ* rq = (_STRU_TCP_CHAT_RQ*)recvData;

	//�жϺ����Ƿ�����
	if (m_mapIdToSocket.count(rq->friendId) > 0) {
		//���ߣ�����������ת��������
		m_pMediator->SendData(recvData, nLen, m_mapIdToSocket[rq->friendId]);
	}
	else {
		//�����ߣ��ظ�send_fail
		_STRU_TCP_CHAT_RS rs;
		rs.result = send_fail;
		rs.friendId = rq->friendId;
		m_pMediator->SendData((char*)&rs, sizeof(rs), lFrom);
	}

}

//������Ӻ�������
void Ckernel::dealAddFriendRq(char* recvData, int nLen, long lFrom)
{
	cout << __func__ << endl;
	//1�����
	_STRU_TCP_ADD_FRIEND_RQ* rq = (_STRU_TCP_ADD_FRIEND_RQ*) recvData;

	//���ݺ���id��ѯ�û�id
	list<string> listRes;
	char sqlBuf[1024] = "";			//�洢sql���
	sprintf(sqlBuf, "select id from t_user where name = '%s';", rq->friendName);						//sprintf:���ַ�����ӡ��ָ���ռ�
	if (!m_sql.SelectMySql(sqlBuf, 1, listRes)) {
		cout << "��ѯ���ݿ�ʧ�ܣ�" << sqlBuf << endl;
		return;
	}

	//3���жϲ�ѯ���
	if (0 == listRes.size()) {
		//û������û�add_friend_no_user
		_STRU_TCP_ADD_FRIEND_RS rs;
		rs.result = add_friend_no_user;
		strcpy(rs.friendName, rq->friendName);
		rs.userId = rq->userId;
		m_pMediator->SendData((char*)&rs,sizeof(rs),lFrom);
	}
	else {
		//ȡ������id
		int friendId = stoi(listRes.front());
		listRes.pop_front();
		//�жϺ����Ƿ�����
		if (m_mapIdToSocket.count(friendId) > 0) {
			//���ߣ�ת����Ӻ�������
			m_pMediator->SendData(recvData, nLen, m_mapIdToSocket[friendId]);
		} 
		else {
			//�����ߣ���Ӻ���ʧ��add_friend_offline
			_STRU_TCP_ADD_FRIEND_RS rs;
			rs.result = add_friend_offline;
			strcpy(rs.friendName,rq->friendName);
			rs.userId = rq->userId;
			rs.friendId = friendId;

			m_pMediator->SendData((char*)&rs, sizeof(rs),lFrom);

		}
	}

}

//������Ӻ��ѻظ�
void Ckernel::dealAddFriendRs(char* recvData, int nLen, long lFrom)
{
	cout << __func__ << endl;
	//1�����
	_STRU_TCP_ADD_FRIEND_RS* rs = (_STRU_TCP_ADD_FRIEND_RS*)recvData;

	//�ж϶Է��Ƿ�ͬ����Ӻ���
	if (add_friend_agree == rs->result) {
		//�Ѻ��ѹ�ϵд�����ݿ⣨���ѹ�ϵ��˫��ģ������Ҫд���Σ�
		char sqlBuf[1024] = "";
		sprintf(sqlBuf, "insert into t_friend values(%d , %d);",rs->friendId,rs->userId);
		if (!m_sql.UpdateMySql(sqlBuf)) {
			cout << "��������ʧ��" << endl;
			return;
		}
		sprintf(sqlBuf, "insert into t_friend values(%d , %d);", rs->userId, rs->friendId);
		if (!m_sql.UpdateMySql(sqlBuf)) {
			cout << "��������ʧ��" << endl;
			return;
		}

		//���º����б�
		getFriendInfo(rs->userId);
	}

	//���ܽ����Σ���Ҫ���͸�������������Ŀͻ���
	m_pMediator->SendData(recvData, nLen, m_mapIdToSocket[rs->userId]);
}

//��ȡ��ǰ��½�û��ĺ�����Ϣ���Լ�����Ϣ
void Ckernel::getFriendInfo(int userId)
{
	cout << __func__ << endl;

	//1�������Լ���id�����ݿ��ѯ�Լ�����Ϣ
	_STRU_TCP_FRIEND_INFO userInfo;
	getUserInfo(userId, &userInfo);

	//2�����Լ�����Ϣ�����ͻ���
	if (m_mapIdToSocket.count(userId) > 0) {
		m_pMediator->SendData((char*)&userInfo, sizeof(userInfo), m_mapIdToSocket[userId]);
	}


	//3�������Լ���id��ѯ���ѵ�id�б�
	list<string> listRes;
	char sqlBuf[1024] = "";			//�洢sql���
	sprintf(sqlBuf, "select idB from t_friend where idA = '%d';", userId);						//sprintf:���ַ�����ӡ��ָ���ռ�
	if (!m_sql.SelectMySql(sqlBuf, 1, listRes)) {
		cout << "��ѯ���ݿ�ʧ�ܣ�" << sqlBuf << endl;
		return;
	}
		//4��������ѯ���
		int friendId = 0;
		_STRU_TCP_FRIEND_INFO friendInfo;
		while (listRes.size() > 0) {
			//5��ȡ�����ѵ�id
			friendId = stoi(listRes.front());
			listRes.pop_front();

			//6�����ݺ��ѵ�id��ѯ���ѵ���Ϣ
			getUserInfo(friendId, &friendInfo);

			//7���Ѻ��ѵ���Ϣ���ؿͻ���
			if (m_mapIdToSocket.count(userId) > 0) {
				m_pMediator->SendData((char*)&friendInfo, sizeof(friendInfo), m_mapIdToSocket[userId]);
			}
			
			//8���жϺ����Ƿ�����
			if (m_mapIdToSocket.count(friendId) > 0) {
				//����������ߣ����Լ�����Ϣ�������ѣ�֪ͨ�����Լ������ߣ�
				m_pMediator->SendData((char*)&userInfo, sizeof(userInfo), m_mapIdToSocket[friendId]);
			}
		}
}

//�����û�id��ѯ�û���Ϣ
void Ckernel::getUserInfo(int id, _STRU_TCP_FRIEND_INFO* info)
{
	cout << __func__ << endl;
	info->id = id;
	//�жϵ�ǰ�û��Ƿ�����
	if (m_mapIdToSocket.count(id) > 0) {
		//����
		info->status = status_online;
	}
	else {
		//����
		info->status = status_offline;
	}

	//�����ݿ��ѯǩ�����ǳơ�iconid
	list<string> listRes;
	char sqlBuf[1024] = "";			//�洢sql���
	sprintf(sqlBuf, "select feeling, name, iconid from t_user where id = '%d';", id);						//sprintf:���ַ�����ӡ��ָ���ռ�
	if (!m_sql.SelectMySql(sqlBuf, 3, listRes)) {
		cout << "��ѯ���ݿ�ʧ�ܣ�" << sqlBuf << endl;
		return;
	}
	//�жϲ�ѯ����Ƿ���ȷ
	if (3 == listRes.size()) {
		//ȡ��ǩ��
		strcpy(info->feeling, listRes.front().c_str());
		listRes.pop_front();

		//ȡ���ǳ�
		strcpy(info->name, listRes.front().c_str());
		listRes.pop_front();

		//ȡ��iconid
		info->iconId = stoi(listRes.front());
		listRes.pop_front();
	}
}
