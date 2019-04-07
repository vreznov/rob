#ifndef KLOGIN_H
#define KLOGIN_H

#include <QDialog>
#include "paramrw.h"

namespace Ui {
class KLogin;
}

class KLogin : public QDialog
{
    Q_OBJECT

public:
    explicit KLogin(QWidget *parent = 0);
    ~KLogin();

    Q_PROPERTY(bool m_passwordCorrect READ getPasswordCorrect)

    //! 当前是否输入了有效的密码
    //! 用于在外部判断进行下一个窗口的弹出
    //! @return: true-密码正确 false-密码错误
    bool getPasswordCorrect();
private:
    bool _passwordCorrect = false;

    void KInit();

    //! 读取默认的密码文件
    bool ReadPasswordFile();
    ParamRW _rw;

private slots:

    //! 确认按钮按下
    void on_pushButton_clicked();

    //! 取消按钮按下
    void on_pushButton_2_clicked();

private:
    Ui::KLogin *ui;

signals:
    //! 密码正确
    void PasswordCorrect();

    //! 密码错误
    void PassWordIncorrect();
};

#endif // KLOGIN_H
