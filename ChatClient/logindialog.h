#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>
#include<QMessageBox>
#include<QCloseEvent>

namespace Ui {
class LoginDialog;
}

class LoginDialog : public QDialog
{
    Q_OBJECT
signals:
    //把注册信息发给kernel
    void SIG_registerCommit(QString name,QString tel,QString password);

    //把登录信息发给kernel
    void SIG_loginCommit(QString tel,QString password);

    //把关闭进程的信号发给kernel
    void SIG_closeLoginDlg();

public:
    explicit LoginDialog(QWidget *parent = 0);
    ~LoginDialog();
    //重写关闭事件
    void closeEvent(QCloseEvent * event);

private slots:
    void on_pb_commit_clicked();

    void on_pb_clear_clicked();

    void on_pb_commit_register_clicked();

    void on_pb_clear_register_clicked();

private:
    Ui::LoginDialog *ui;
};

#endif // LOGINDIALOG_H
