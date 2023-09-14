#include "logindialog.h"
#include "ui_logindialog.h"

LoginDialog::LoginDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LoginDialog)
{
    ui->setupUi(this);
}

LoginDialog::~LoginDialog()
{
    delete ui;
}

//重写关闭事件
void LoginDialog::closeEvent(QCloseEvent *event)
{
    //给kernel发信号，关闭进程
    Q_EMIT SIG_closeLoginDlg();
}

//登录提交
void LoginDialog::on_pb_commit_clicked()
{
    //1、获取用户输入的数据
    QString password = ui->le_password->text();
    QString tel = ui->le_tel->text();
    QString passwordTmp = password;
    QString telTmp = tel;

    //2、校验
    //2.1、判断是否是空字符串或者是全空格
    if(password.isEmpty()||tel.isEmpty()
            ||passwordTmp.remove(" ").isEmpty()
            ||telTmp.remove(" ").isEmpty()){
        QMessageBox::about(this,"提示","输入为空");
        return;
    }

    //2.2、检查长度是否合法（昵称<20，电话=11，密码<20）
    if(tel.length() != 11 || password.length() >= 20){
        QMessageBox::about(this,"提示","长度过长");
        return;
    }
    //2.3、检查内容是否合法（电话必须全是数字、呢称和密码只能是数字、字母、下划线）
        //把登录信息发给kernel
       Q_EMIT SIG_loginCommit(tel, password);
}

void LoginDialog::on_pb_clear_clicked()
{
    ui->le_password->setText("");
    ui->le_tel->setText("");
}

void LoginDialog::on_pb_commit_register_clicked()
{
    //1、获取用户输入的数据
    QString name = ui->le_name_register->text();
    QString password = ui->le_password_register->text();
    QString tel = ui->le_tel_register->text();
    QString nameTmp = name;
    QString passwordTmp = password;
    QString telTmp = tel;

    //2、校验
    //2.1、判断是否是空字符串或者是全空格
    if(name.isEmpty()||password.isEmpty()||tel.isEmpty()
            ||nameTmp.remove(" ").isEmpty()||passwordTmp.remove(" ").isEmpty()
            ||telTmp.remove(" ").isEmpty()){
        QMessageBox::about(this,"提示","输入为空");
        return;
    }

    //2.2、检查长度是否合法（昵称<20，电话=11，密码<20）
    if(name.length() >= 20 || tel.length() != 11 || password.length() >= 20){
        QMessageBox::about(this,"提示","长度过长");
        return;
    }
    //2.3、检查内容是否合法（电话必须全是数字、呢称和密码只能是数字、字母、下划线）

    //3、把注册信息发给kernel
   Q_EMIT SIG_registerCommit(name, tel, password);

}

void LoginDialog::on_pb_clear_register_clicked()
{
    ui->le_password_register->setText("");
    ui->le_name_register->setText("");
    ui->le_tel_register->setText("");
}
