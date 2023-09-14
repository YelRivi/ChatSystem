#pragma once

#define _DEF_UDP_PORT 45678
#define _DEF_TCP_PORT 45678
#define _DEF_TCP_SERVER_IP ("192.168.39.169")
#define _DEF_NAME_MAX 100
#define _DEF_CONTENT_LENGTH 2048
#define _DEF_PROTOCOL_COUNT (15)


typedef int PackType;

//定义协议头
#define _DEF_PROTOCOL_BASE (1000)

//上线请求
#define _DEF_UDP_ONLINE_RQ  (_DEF_PROTOCOL_BASE + 1)

//上线回复
#define _DEF_UDP_ONLINE_RS  (_DEF_PROTOCOL_BASE + 2)

//聊天请求
#define _DEF_UDP_CHAT_RQ  (_DEF_PROTOCOL_BASE + 3)

//下线请求
#define _DEF_UDP_OFFLINE_RQ  (_DEF_PROTOCOL_BASE + 4)


//注册请求
#define _DEF_TCP_REGISTER_RQ (_DEF_PROTOCOL_BASE + 5)

//注册回复
#define _DEF_TCP_REGISTER_RS (_DEF_PROTOCOL_BASE + 6)

//登录请求
#define _DEF_TCP_LOGIN_RQ (_DEF_PROTOCOL_BASE + 7)

//登录回复
#define _DEF_TCP_LOGIN_RS (_DEF_PROTOCOL_BASE + 8)

//好友信息
#define _DEF_TCP_FRIEND_INFO (_DEF_PROTOCOL_BASE + 9)

//聊天请求
#define _DEF_TCP_CHAT_RQ (_DEF_PROTOCOL_BASE + 10)

//聊天回复
#define _DEF_TCP_CHAT_RS (_DEF_PROTOCOL_BASE + 11)

//添加好友请求
#define _DEF_TCP_ADD_FRIEND_RQ (_DEF_PROTOCOL_BASE + 12)

//添加好友回复
#define _DEF_TCP_ADD_FRIEND_RS (_DEF_PROTOCOL_BASE + 13)

//下线请求
#define _DEF_TCP_OFFLINE_RQ (_DEF_PROTOCOL_BASE + 14)

//定义结果
//注册结果
#define register_success    (0)
#define register_tel_used    (1)
#define register_name_used    (2)
//登录结果
#define login_success   (0)
#define login_no_tel  (1)
#define login_password_error  (2)
//状态
#define status_online   (0)
#define status_offline   (1)
//聊天结果
#define send_success    (0)
#define send_fail    (1)
//添加好友结果
#define add_friend_no_user  (0)
#define add_friend_offline  (1)
#define add_friend_agree  (2)
#define add_friend_refuse  (3)



//协议包
//上线请求、上线回复：协议头、ip、主机名
struct _STRU_ONLINE {
    _STRU_ONLINE() :type(_DEF_UDP_ONLINE_RQ)/*,ip(0)*/ {
        memset(name, 0, _DEF_NAME_MAX);
    }

    PackType type;
    char name[_DEF_NAME_MAX];
};
//聊天请求：协议头、ip、聊天内容
struct _STRU_CHAT_RQ {
    PackType type;
    _STRU_CHAT_RQ() :type(_DEF_UDP_CHAT_RQ)/*,ip(0)*/ {
        memset(content, 0, _DEF_CONTENT_LENGTH);
    }

    //long ip;
    char content[_DEF_CONTENT_LENGTH];
};

//下线请求：协议头、ip
struct _STRU_OFFLINE_RQ {
    PackType type;
    _STRU_OFFLINE_RQ() :type(_DEF_UDP_OFFLINE_RQ) {
    }
};
//注册请求
typedef struct _STRU_TCP_REGISTER_RQ {
    _STRU_TCP_REGISTER_RQ() :
        type(_DEF_TCP_REGISTER_RQ) {
        memset(tel, 0, _DEF_NAME_MAX);
        memset(name, 0, _DEF_NAME_MAX);
        memset(password, 0, _DEF_NAME_MAX);
    }
    PackType type;
    char tel[_DEF_NAME_MAX];
    char name[_DEF_NAME_MAX];
    char password[_DEF_NAME_MAX];
}_STRU_TCP_REGISTER_RQ;

//注册回复：协议头，注册结果（成功，电话已被注册，昵称已被使用）
typedef struct _STRU_TCP_REGISTER_RS {
    _STRU_TCP_REGISTER_RS() :type(_DEF_TCP_REGISTER_RS), result(register_name_used)
    {}
    PackType type;
    int result;
}_STRU_TCP_REGISTER_RS;

//登录请求：协议头、电话、密码
typedef struct _STRU_TCP_LOGIN_RQ {
    _STRU_TCP_LOGIN_RQ() :type(_DEF_TCP_LOGIN_RQ) {
        memset(tel, 0, _DEF_NAME_MAX);
        memset(password, 0, _DEF_NAME_MAX);
    }
    PackType type;
    char tel[_DEF_NAME_MAX];
    char password[_DEF_NAME_MAX];
}_STRU_TCP_LOGIN_RQ;

//登录回复：协议头、登陆结果（成功、电话未注册、密码错误）
typedef struct _STRU_TCP_LOGIN_RS {
    _STRU_TCP_LOGIN_RS() :type(_DEF_TCP_LOGIN_RS), result(login_password_error),userId(0)
    {}
    PackType type;
    int userId;
    int result;
}_STRU_TCP_LOGIN_RS;

//好友信息：协议头、id、昵称、头像id、签名、状态
typedef struct _STRU_TCP_FRIEND_INFO {
    _STRU_TCP_FRIEND_INFO() :type(_DEF_TCP_FRIEND_INFO),
        id(0), iconId(0), status(status_offline) {
        memset(name, 0, _DEF_NAME_MAX);
        memset(feeling, 0, _DEF_NAME_MAX);
    }
    PackType type;
    int id;
    int iconId;
    int status;
    char name[_DEF_NAME_MAX];
    char feeling[_DEF_NAME_MAX];
}_STRU_TCP_FRIEND_INFO;


//聊天请求：协议头、聊天内容、好友id、自己的id
typedef struct _STRU_TCP_CHAT_RQ {
    _STRU_TCP_CHAT_RQ() :type(_DEF_TCP_CHAT_RQ), userId(0), friendId(0) {
        memset(content, 0, _DEF_CONTENT_LENGTH);
    }
    PackType type;
    int userId;
    int friendId;
    char content[_DEF_CONTENT_LENGTH];

}_STRU_TCP_CHAT_RQ;


//聊天回复：协议头、聊天结果、好友ID
typedef struct _STRU_TCP_CHAT_RS {
    _STRU_TCP_CHAT_RS() :type(_DEF_TCP_CHAT_RS), result(send_fail), friendId(0)
    {}
    PackType type;
    int friendId;
    int result;
}_STRU_TCP_CHAT_RS;

//添加好友请求：协议头、好友昵称、自己的id、自己的昵称
typedef struct _STRU_TCP_ADD_FRIEND_RQ {
    _STRU_TCP_ADD_FRIEND_RQ() :type(_DEF_TCP_ADD_FRIEND_RQ), userId(0) {
        memset(userName, 0, _DEF_NAME_MAX);
        memset(friendName, 0, _DEF_NAME_MAX);
    }
    PackType type;
    int userId;
    char userName[_DEF_NAME_MAX];
    char friendName[_DEF_NAME_MAX];
}_STRU_TCP_ADD_FRIEND_RQ;


//添加好友回复：协议头、添加结果（好友不存在、好友不在线、同意、拒绝）
typedef struct _STRU_TCP_ADD_FRIEND_RS {
    _STRU_TCP_ADD_FRIEND_RS() :
        type(_DEF_TCP_ADD_FRIEND_RS), result(add_friend_refuse), userId(0), friendId(0) {
        memset(friendName, 0, _DEF_NAME_MAX);
    }
    PackType type;
    int result;
    int userId;
    int friendId;
    char friendName[_DEF_NAME_MAX];
}_STRU_TCP_ADD_FRIEND_RS;

//下线请求
typedef struct _STRU_TCP_OFFLINE_RQ {
    _STRU_TCP_OFFLINE_RQ() :type(_DEF_TCP_OFFLINE_RQ), userId(0)
    {}
    PackType type;
    int userId;
}_STRU_TCP_OFFLINE_RQ;

//数据库表
//用户信息表t_user: id(int,自增，主键)、tel、name、password、iconid、feeling
//好友关系表t_friend: idA、idB、（为了查找方便和快速，采用双向）
