#include "chatdialog.h"
#include "ui_chatdialog.h"
#include<QTime>

ChatDialog::ChatDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ChatDialog)
{
    ui->setupUi(this);
}

ChatDialog::~ChatDialog()
{
    delete ui;
}
//设置聊天窗口title
void ChatDialog::setChatDialogInfo(QString ip,int id)
{
    //保存信息
    m_ip = ip;
    m_id = id;
    //设置窗口title
    setWindowTitle(QString("与 %1 的聊天窗口").arg(m_ip));
}

//把聊天内容设置到窗口上
void ChatDialog::setContent(QString content)
{
    ui->tb_chat->append(QString("%1 %2").arg(m_ip).arg(QTime::currentTime().toString("hh:mm:ss")));
    ui->tb_chat->append(content);
}

//获取好友的昵称
QString ChatDialog::getName()
{
    return m_ip;
}

void ChatDialog::on_pb_send_clicked()
{
    //获取输入的文本
    QString content = ui->te_chat->toPlainText();//获取纯文本

    //判断是否是空，或者全是空格
    if(content.isEmpty()||content.remove(" ").isEmpty()){
        ui->te_chat->clear();
        return;
    }
    //获取带格式的文本
    content = ui->te_chat->toHtml();

    //清空编辑窗口
    ui->te_chat->clear();

    //把文本显示到浏览窗口上
    ui->tb_chat->append(QString("我 %1").arg(QTime::currentTime().toString("hh:mm:ss")));
    ui->tb_chat->append(content);

    if(-1 == m_id){

        //UDP:把数据发给kernel
        Q_EMIT SIG_sendMsg(content,m_ip);
    }else{
        //TCP:把聊天内容数据发给kernel
        Q_EMIT SIG_sendTcpMsg(content,m_id);
    }

}
