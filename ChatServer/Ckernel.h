#pragma once
#include<iostream>
#include"mediator/INetMediator.h"
#include"CMySql.h"
#include<map>

using namespace std;
//定义函数指针
class Ckernel;
typedef void(Ckernel::* pFun)(char* recvData, int nLen, long lFrom);

class Ckernel
{
public:
	Ckernel();
	~Ckernel();


	//打开服务器
	bool startServer();;
	//关闭服务器
	void closeServer();

	//初始化协议头数组
	void setProtocolMap();

	//获取当前登陆用户的好友信息和自己的信息
	void getFriendInfo(int userId);

	//根据用户id查询用户信息
	void getUserInfo(int id,_STRU_TCP_FRIEND_INFO* info);

	//处理所有接收到的数据
	void dealReadyData(char* recvData, int nLen, long lFrom);

	//处理注册请求
	void dealRegisterRq(char* recvData, int nLen, long lFrom);

	//处理登录请求
	void dealLoginRq(char* recvData, int nLen, long lFrom);

	//处理下线请求
	void dealOfflineRq(char* recvData, int nLen, long lFrom);

	//处理聊天请求
	void dealChatRq(char* recvData, int nLen, long lFrom);

	//处理添加好友请求
	void dealAddFriendRq(char* recvData, int nLen, long lFrom);

	//处理添加好友回复
	void dealAddFriendRs(char* recvData, int nLen, long lFrom);


	static Ckernel* pKernel;
private:
	//中介指针
	INetMediator* m_pMediator;

	//定义数据库的对象
	CMySql m_sql;

	//定义函数指针数组
	pFun m_mapPackTypeToFun [_DEF_PROTOCOL_COUNT];

	//保存当前登陆成功的用户id和socket
	//作用：1、给客户端发消息的时候可以取出客户端的socket;2、判断客户端是否在线）
	map<int, SOCKET> m_mapIdToSocket;

};

