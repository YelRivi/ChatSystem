#ifndef CHATDIALOG_H
#define CHATDIALOG_H

#include <QDialog>

namespace Ui {
class ChatDialog;
}

class ChatDialog : public QDialog
{
    Q_OBJECT
signals:
    //UDP:把聊天内容发给kernel
    void SIG_sendMsg(QString content,QString ip);

    //TCP:把聊天内容发给kernel
    void SIG_sendTcpMsg(QString content,int id);

public:
    explicit ChatDialog(QWidget *parent = 0);
    ~ChatDialog();

    //设置聊天窗口title
    void setChatDialogInfo(QString ip,int id = -1);

    //把聊天内容设置到窗口上
    void setContent(QString content);

    //获取好友的昵称
    QString getName();

private slots:
    void on_pb_send_clicked();

private:
    Ui::ChatDialog *ui;
    //UDP的时候存ip，TCP的时候存名字
    QString m_ip;
    int m_id;
};

#endif // CHATDIALOG_H
