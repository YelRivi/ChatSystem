#pragma once

#define _DEF_UDP_PORT 45678
#define _DEF_TCP_PORT 45678
#define _DEF_TCP_SERVER_IP ("192.168.39.169")
#define _DEF_NAME_MAX 100
#define _DEF_CONTENT_LENGTH 2048
#define _DEF_PROTOCOL_COUNT (15)


typedef int PackType;

//����Э��ͷ
#define _DEF_PROTOCOL_BASE (1000)

//��������
#define _DEF_UDP_ONLINE_RQ  (_DEF_PROTOCOL_BASE + 1)

//���߻ظ�
#define _DEF_UDP_ONLINE_RS  (_DEF_PROTOCOL_BASE + 2)

//��������
#define _DEF_UDP_CHAT_RQ  (_DEF_PROTOCOL_BASE + 3)

//��������
#define _DEF_UDP_OFFLINE_RQ  (_DEF_PROTOCOL_BASE + 4)


//ע������
#define _DEF_TCP_REGISTER_RQ (_DEF_PROTOCOL_BASE + 5)

//ע��ظ�
#define _DEF_TCP_REGISTER_RS (_DEF_PROTOCOL_BASE + 6)

//��¼����
#define _DEF_TCP_LOGIN_RQ (_DEF_PROTOCOL_BASE + 7)

//��¼�ظ�
#define _DEF_TCP_LOGIN_RS (_DEF_PROTOCOL_BASE + 8)

//������Ϣ
#define _DEF_TCP_FRIEND_INFO (_DEF_PROTOCOL_BASE + 9)

//��������
#define _DEF_TCP_CHAT_RQ (_DEF_PROTOCOL_BASE + 10)

//����ظ�
#define _DEF_TCP_CHAT_RS (_DEF_PROTOCOL_BASE + 11)

//��Ӻ�������
#define _DEF_TCP_ADD_FRIEND_RQ (_DEF_PROTOCOL_BASE + 12)

//��Ӻ��ѻظ�
#define _DEF_TCP_ADD_FRIEND_RS (_DEF_PROTOCOL_BASE + 13)

//��������
#define _DEF_TCP_OFFLINE_RQ (_DEF_PROTOCOL_BASE + 14)

//������
//ע����
#define register_success    (0)
#define register_tel_used    (1)
#define register_name_used    (2)
//��¼���
#define login_success   (0)
#define login_no_tel  (1)
#define login_password_error  (2)
//״̬
#define status_online   (0)
#define status_offline   (1)
//������
#define send_success    (0)
#define send_fail    (1)
//��Ӻ��ѽ��
#define add_friend_no_user  (0)
#define add_friend_offline  (1)
#define add_friend_agree  (2)
#define add_friend_refuse  (3)



//Э���
//�����������߻ظ���Э��ͷ��ip��������
struct _STRU_ONLINE {
    _STRU_ONLINE() :type(_DEF_UDP_ONLINE_RQ)/*,ip(0)*/ {
        memset(name, 0, _DEF_NAME_MAX);
    }

    PackType type;
    char name[_DEF_NAME_MAX];
};
//��������Э��ͷ��ip����������
struct _STRU_CHAT_RQ {
    PackType type;
    _STRU_CHAT_RQ() :type(_DEF_UDP_CHAT_RQ)/*,ip(0)*/ {
        memset(content, 0, _DEF_CONTENT_LENGTH);
    }

    //long ip;
    char content[_DEF_CONTENT_LENGTH];
};

//��������Э��ͷ��ip
struct _STRU_OFFLINE_RQ {
    PackType type;
    _STRU_OFFLINE_RQ() :type(_DEF_UDP_OFFLINE_RQ) {
    }
};
//ע������
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

//ע��ظ���Э��ͷ��ע�������ɹ����绰�ѱ�ע�ᣬ�ǳ��ѱ�ʹ�ã�
typedef struct _STRU_TCP_REGISTER_RS {
    _STRU_TCP_REGISTER_RS() :type(_DEF_TCP_REGISTER_RS), result(register_name_used)
    {}
    PackType type;
    int result;
}_STRU_TCP_REGISTER_RS;

//��¼����Э��ͷ���绰������
typedef struct _STRU_TCP_LOGIN_RQ {
    _STRU_TCP_LOGIN_RQ() :type(_DEF_TCP_LOGIN_RQ) {
        memset(tel, 0, _DEF_NAME_MAX);
        memset(password, 0, _DEF_NAME_MAX);
    }
    PackType type;
    char tel[_DEF_NAME_MAX];
    char password[_DEF_NAME_MAX];
}_STRU_TCP_LOGIN_RQ;

//��¼�ظ���Э��ͷ����½������ɹ����绰δע�ᡢ�������
typedef struct _STRU_TCP_LOGIN_RS {
    _STRU_TCP_LOGIN_RS() :type(_DEF_TCP_LOGIN_RS), result(login_password_error),userId(0)
    {}
    PackType type;
    int userId;
    int result;
}_STRU_TCP_LOGIN_RS;

//������Ϣ��Э��ͷ��id���ǳơ�ͷ��id��ǩ����״̬
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


//��������Э��ͷ���������ݡ�����id���Լ���id
typedef struct _STRU_TCP_CHAT_RQ {
    _STRU_TCP_CHAT_RQ() :type(_DEF_TCP_CHAT_RQ), userId(0), friendId(0) {
        memset(content, 0, _DEF_CONTENT_LENGTH);
    }
    PackType type;
    int userId;
    int friendId;
    char content[_DEF_CONTENT_LENGTH];

}_STRU_TCP_CHAT_RQ;


//����ظ���Э��ͷ��������������ID
typedef struct _STRU_TCP_CHAT_RS {
    _STRU_TCP_CHAT_RS() :type(_DEF_TCP_CHAT_RS), result(send_fail), friendId(0)
    {}
    PackType type;
    int friendId;
    int result;
}_STRU_TCP_CHAT_RS;

//��Ӻ�������Э��ͷ�������ǳơ��Լ���id���Լ����ǳ�
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


//��Ӻ��ѻظ���Э��ͷ����ӽ�������Ѳ����ڡ����Ѳ����ߡ�ͬ�⡢�ܾ���
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

//��������
typedef struct _STRU_TCP_OFFLINE_RQ {
    _STRU_TCP_OFFLINE_RQ() :type(_DEF_TCP_OFFLINE_RQ), userId(0)
    {}
    PackType type;
    int userId;
}_STRU_TCP_OFFLINE_RQ;

//���ݿ��
//�û���Ϣ��t_user: id(int,����������)��tel��name��password��iconid��feeling
//���ѹ�ϵ��t_friend: idA��idB����Ϊ�˲��ҷ���Ϳ��٣�����˫��
