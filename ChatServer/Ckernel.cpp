#include "Ckernel.h"
#include"TcpServerMediator.h"
#include"packDef.h"

//静态成员变量类外初始化
Ckernel* Ckernel::pKernel = nullptr;

Ckernel::Ckernel()
{
	//初始化协议头数组
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
	//打开网络
	if (!m_pMediator->OpenNet()) {
		cout << "打开网络失败" << endl;
		return false;
	}

	//连接数据库
	if (!m_sql.ConnectMySql("127.0.0.1","root","schoolidol","improject")) {
		cout << "连接数据库失败" << endl;
		return false;
	}
}

//关闭服务器
void Ckernel::closeServer()
{
	cout << __func__ << endl;
	if (m_pMediator) {
		m_pMediator->CloseNet();
		delete m_pMediator;
		m_pMediator = nullptr;
	}

	//断开和数据库的连接
	m_sql.DisConnect();
}

//绑定协议头和处理函数 
void Ckernel::setProtocolMap()
{
	cout << __func__ << endl;
	//数组初始化成0
	memset(m_mapPackTypeToFun, 0, sizeof(m_mapPackTypeToFun));

	//绑定协议头和处理函数
	m_mapPackTypeToFun[_DEF_TCP_REGISTER_RQ - _DEF_PROTOCOL_BASE - 1 ]  = &Ckernel::dealRegisterRq;
	m_mapPackTypeToFun[_DEF_TCP_LOGIN_RQ - _DEF_PROTOCOL_BASE - 1]	    = &Ckernel::dealLoginRq;
	m_mapPackTypeToFun[_DEF_TCP_OFFLINE_RQ - _DEF_PROTOCOL_BASE - 1]    = &Ckernel::dealOfflineRq;
	m_mapPackTypeToFun[_DEF_TCP_CHAT_RQ       - _DEF_PROTOCOL_BASE - 1] = &Ckernel::dealChatRq;
	m_mapPackTypeToFun[_DEF_TCP_ADD_FRIEND_RQ - _DEF_PROTOCOL_BASE - 1] = &Ckernel::dealAddFriendRq;
	m_mapPackTypeToFun[_DEF_TCP_ADD_FRIEND_RS - _DEF_PROTOCOL_BASE - 1] = &Ckernel::dealAddFriendRs;

}

//处理所有接收到的数据
void Ckernel::dealReadyData(char* recvData, int nLen, long lFrom)
{

	cout << __func__ << endl;
	//取出协议头
	PackType type = *(PackType*)recvData;//为什么？

	//根据协议头走到不同的处理流程
	//计算数组下标
	int index = type - _DEF_PROTOCOL_BASE - 1;
	//校验数组下标是否在合法范围内
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

	//删除掉申请的空间
	delete[] recvData;

}

//处理注册请求
void Ckernel::dealRegisterRq(char* recvData, int nLen, long lFrom) {

	cout << __func__ << endl;
	//拆包
	_STRU_TCP_REGISTER_RQ* rq = (_STRU_TCP_REGISTER_RQ*)recvData;
	
	//校验数据合法性（与客户端校验规则一致）
	// 
	//根据电话查询数据库
	list<string> listRes;
	char sqlBuf[1024] = "";			//存储sql语句
	sprintf(sqlBuf,"select tel from t_user where tel = '%s'",rq->tel);						//sprintf:把字符串打印到指定空间
	if (!m_sql.SelectMySql(sqlBuf,1 ,listRes)) {
		cout << "查询数据库失败：" << sqlBuf << endl;
		return;
	}
	//判断查询结果
	_STRU_TCP_REGISTER_RS rs;
	if (listRes.size() > 0) {
		//注册失败，电话号码已被注册
		rs.result = register_tel_used;
	}
	else {
		//根据昵称查询数据库
		sprintf(sqlBuf, "select name from t_user where name = '%s'", rq->name);						//sprintf:把字符串打印到指定空间
		if (!m_sql.SelectMySql(sqlBuf, 1, listRes)) {
			cout << "查询数据库失败："<< sqlBuf << endl;
			return;
		}
		//判断查询结果
		if (listRes.size() > 0) {
			//注册失败，昵称已被注册
			rs.result = register_name_used;
		}
		else {
			//把用户信息写入数据库
			sprintf(sqlBuf, "insert into t_user (name, tel, password, feeling, iconid) values ('%s', '%s', '%s', '你好！',13);",rq->name,rq->tel,rq->password);
			if (!m_sql.UpdateMySql(sqlBuf)) {
				cout << "插入数据库失败：" << sqlBuf << endl;
				return;
			}

			//注册成功
			rs.result = register_success;
		}
	}

	//给客户端发回注册结果
	m_pMediator->SendData((char*)&rs,sizeof(rs),lFrom);

}

//登录请求
void Ckernel::dealLoginRq(char* recvData, int nLen, long lFrom)
{
	cout << __func__ << endl;
	//拆包
	_STRU_TCP_LOGIN_RQ* rq = (_STRU_TCP_LOGIN_RQ*)recvData;

	//校验数据合法性（与客户端校验规则一致）
	
	//根据电话查id和密码
	list<string> listRes;
	char sqlBuf[1024] = "";			//存储sql语句
	sprintf(sqlBuf, "select id, password from t_user where tel = '%s';", rq->tel);						//sprintf:把字符串打印到指定空间
	if (!m_sql.SelectMySql(sqlBuf, 2, listRes)) {
		cout << "查询数据库失败：" << sqlBuf << endl;
		return;
	}
	//判断查询结果
	_STRU_TCP_LOGIN_RS rs;
	//判断查找结果
	if (0 == listRes.size()) {
		//没有查到，登录失败,login_no_tel
		rs.result = login_no_tel;
	}
	else {
		//取出id
		int id = stoi(listRes.front());
		listRes.pop_front();				//每次都是从listRes的头开始取，取了之后要删除

		//取出密码
		string password = listRes.front();			//取出第一个字母
		listRes.pop_front();						//弹出第一个字母

		//比较密码是否相同
		//使用strcmp比较字符串
		if (0 == strcmp(password.c_str(),rq->password)) {	//password需要转换成标准char类型
			//相同，登录成功	login_success
			rs.result = login_success;
			rs.userId = id;

			//登录结果发给客户端
			m_pMediator->SendData((char*)&rs, sizeof(rs), lFrom);

			//保存当前登录用户的socket和id
			m_mapIdToSocket[id] = lFrom;

			// 获取好友id列表
			// 判断每个好友是否在线，在线就通知

			//获取当前登录用户的好友信息和自己的信息
			getFriendInfo(id);

			return;
		}
		else {
			//不同，登录失败	login_password_error
			rs.result = login_password_error;
		}
	}

	//登录结果发给客户端
	m_pMediator->SendData((char*)&rs, sizeof(rs), lFrom);
}

//处理下线请求
void Ckernel::dealOfflineRq(char* recvData, int nLen, long lFrom)
{

	cout << __func__ << endl;
	//拆包
	_STRU_TCP_OFFLINE_RQ* rq = (_STRU_TCP_OFFLINE_RQ*)recvData;

	//根据用户id查询好友id列表
	list<string> listRes;
	char sqlBuf[1024] = "";			//存储sql语句
	sprintf(sqlBuf, "select idB from t_friend where idA = '%d';", rq->userId);						//sprintf:把字符串打印到指定空间
	if (!m_sql.SelectMySql(sqlBuf, 1, listRes)) {
		cout << "查询数据库失败：" << sqlBuf << endl;
		return;
	}

	//遍历好友id列表
	int friendId = 0;
	_STRU_TCP_FRIEND_INFO friendInfo;
	while (listRes.size() > 0) {

		//取出好友id
		friendId = stoi(listRes.front());
		listRes.pop_front();

		//判断好友是否在线
		if (m_mapIdToSocket.count(friendId) > 0) {
			//如果好友在线，转发下线请求
			m_pMediator->SendData(recvData,nLen, m_mapIdToSocket[friendId]);
		}
	}
	//把自己从map中移除
	if (m_mapIdToSocket.count(rq->userId) > 0) {
		closesocket(m_mapIdToSocket[rq->userId]);
		m_mapIdToSocket.erase(rq->userId);
	}
}

//处理聊天请求
void Ckernel::dealChatRq(char* recvData, int nLen, long lFrom)
{
	cout << __func__ << endl;
	//拆包
	_STRU_TCP_CHAT_RQ* rq = (_STRU_TCP_CHAT_RQ*)recvData;

	//判断好友是否在线
	if (m_mapIdToSocket.count(rq->friendId) > 0) {
		//在线，把聊天请求转发给好友
		m_pMediator->SendData(recvData, nLen, m_mapIdToSocket[rq->friendId]);
	}
	else {
		//不在线，回复send_fail
		_STRU_TCP_CHAT_RS rs;
		rs.result = send_fail;
		rs.friendId = rq->friendId;
		m_pMediator->SendData((char*)&rs, sizeof(rs), lFrom);
	}

}

//处理添加好友请求
void Ckernel::dealAddFriendRq(char* recvData, int nLen, long lFrom)
{
	cout << __func__ << endl;
	//1、拆包
	_STRU_TCP_ADD_FRIEND_RQ* rq = (_STRU_TCP_ADD_FRIEND_RQ*) recvData;

	//根据好友id查询用户id
	list<string> listRes;
	char sqlBuf[1024] = "";			//存储sql语句
	sprintf(sqlBuf, "select id from t_user where name = '%s';", rq->friendName);						//sprintf:把字符串打印到指定空间
	if (!m_sql.SelectMySql(sqlBuf, 1, listRes)) {
		cout << "查询数据库失败：" << sqlBuf << endl;
		return;
	}

	//3、判断查询结果
	if (0 == listRes.size()) {
		//没有这个用户add_friend_no_user
		_STRU_TCP_ADD_FRIEND_RS rs;
		rs.result = add_friend_no_user;
		strcpy(rs.friendName, rq->friendName);
		rs.userId = rq->userId;
		m_pMediator->SendData((char*)&rs,sizeof(rs),lFrom);
	}
	else {
		//取出好友id
		int friendId = stoi(listRes.front());
		listRes.pop_front();
		//判断好友是否在线
		if (m_mapIdToSocket.count(friendId) > 0) {
			//在线，转发添加好友请求
			m_pMediator->SendData(recvData, nLen, m_mapIdToSocket[friendId]);
		} 
		else {
			//不在线，添加好友失败add_friend_offline
			_STRU_TCP_ADD_FRIEND_RS rs;
			rs.result = add_friend_offline;
			strcpy(rs.friendName,rq->friendName);
			rs.userId = rq->userId;
			rs.friendId = friendId;

			m_pMediator->SendData((char*)&rs, sizeof(rs),lFrom);

		}
	}

}

//处理添加好友回复
void Ckernel::dealAddFriendRs(char* recvData, int nLen, long lFrom)
{
	cout << __func__ << endl;
	//1、拆包
	_STRU_TCP_ADD_FRIEND_RS* rs = (_STRU_TCP_ADD_FRIEND_RS*)recvData;

	//判断对方是否同意添加好友
	if (add_friend_agree == rs->result) {
		//把好友关系写入数据库（好友关系是双向的，因此需要写两次）
		char sqlBuf[1024] = "";
		sprintf(sqlBuf, "insert into t_friend values(%d , %d);",rs->friendId,rs->userId);
		if (!m_sql.UpdateMySql(sqlBuf)) {
			cout << "插入数据失败" << endl;
			return;
		}
		sprintf(sqlBuf, "insert into t_friend values(%d , %d);", rs->userId, rs->friendId);
		if (!m_sql.UpdateMySql(sqlBuf)) {
			cout << "插入数据失败" << endl;
			return;
		}

		//更新好友列表
		getFriendInfo(rs->userId);
	}

	//不管结果如何，都要发送给发出好友请求的客户端
	m_pMediator->SendData(recvData, nLen, m_mapIdToSocket[rs->userId]);
}

//获取当前登陆用户的好友信息和自己的信息
void Ckernel::getFriendInfo(int userId)
{
	cout << __func__ << endl;

	//1、根据自己的id从数据库查询自己的信息
	_STRU_TCP_FRIEND_INFO userInfo;
	getUserInfo(userId, &userInfo);

	//2、把自己的信息发给客户端
	if (m_mapIdToSocket.count(userId) > 0) {
		m_pMediator->SendData((char*)&userInfo, sizeof(userInfo), m_mapIdToSocket[userId]);
	}


	//3、根据自己的id查询好友的id列表
	list<string> listRes;
	char sqlBuf[1024] = "";			//存储sql语句
	sprintf(sqlBuf, "select idB from t_friend where idA = '%d';", userId);						//sprintf:把字符串打印到指定空间
	if (!m_sql.SelectMySql(sqlBuf, 1, listRes)) {
		cout << "查询数据库失败：" << sqlBuf << endl;
		return;
	}
		//4、遍历查询结果
		int friendId = 0;
		_STRU_TCP_FRIEND_INFO friendInfo;
		while (listRes.size() > 0) {
			//5、取出好友的id
			friendId = stoi(listRes.front());
			listRes.pop_front();

			//6、根据好友的id查询好友的信息
			getUserInfo(friendId, &friendInfo);

			//7、把好友的信息发回客户端
			if (m_mapIdToSocket.count(userId) > 0) {
				m_pMediator->SendData((char*)&friendInfo, sizeof(friendInfo), m_mapIdToSocket[userId]);
			}
			
			//8、判断好友是否在线
			if (m_mapIdToSocket.count(friendId) > 0) {
				//如果好友在线，把自己的信息发给好友（通知好友自己已上线）
				m_pMediator->SendData((char*)&userInfo, sizeof(userInfo), m_mapIdToSocket[friendId]);
			}
		}
}

//根据用户id查询用户信息
void Ckernel::getUserInfo(int id, _STRU_TCP_FRIEND_INFO* info)
{
	cout << __func__ << endl;
	info->id = id;
	//判断当前用户是否在线
	if (m_mapIdToSocket.count(id) > 0) {
		//在线
		info->status = status_online;
	}
	else {
		//下线
		info->status = status_offline;
	}

	//从数据库查询签名、昵称、iconid
	list<string> listRes;
	char sqlBuf[1024] = "";			//存储sql语句
	sprintf(sqlBuf, "select feeling, name, iconid from t_user where id = '%d';", id);						//sprintf:把字符串打印到指定空间
	if (!m_sql.SelectMySql(sqlBuf, 3, listRes)) {
		cout << "查询数据库失败：" << sqlBuf << endl;
		return;
	}
	//判断查询结果是否正确
	if (3 == listRes.size()) {
		//取出签名
		strcpy(info->feeling, listRes.front().c_str());
		listRes.pop_front();

		//取出昵称
		strcpy(info->name, listRes.front().c_str());
		listRes.pop_front();

		//取出iconid
		info->iconId = stoi(listRes.front());
		listRes.pop_front();
	}
}
