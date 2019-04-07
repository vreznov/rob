#include "klogin.h"
#include "ui_klogin.h"
#include <qdebug.h>
#include <QMessageBox>

KLogin::KLogin(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::KLogin)
{
    ui->setupUi(this);
    KInit();
}

KLogin::~KLogin()
{
    delete ui;
}

bool KLogin::getPasswordCorrect()
{
    return _passwordCorrect;
}

void KLogin::KInit()
{
    _rw.SetFilePath("./debug/pw.txt");
    try
    {
        _rw.ReadAllParam();
    }
    catch(ExceptionBase ex)
    {
        QString err = ex.m_errInfo.c_str();
        qDebug()<<err<<endl;
    }
}

void KLogin::on_pushButton_clicked()
{
    //获取账户密码
    QString name = ui->lineEdit->text();
    QString psw = ui->lineEdit_2->text();
    qDebug()<<"name="<<name<<endl;
    qDebug()<<"psw="<<psw<<endl;

    QString sname = _rw.GetValueByName(name.toStdString()).c_str(); //与库中的数据对比
    if(sname == psw && sname != "" && psw != "") //密码一致切均不为空
    {
        qDebug()<<"password correct"<<endl;
        emit PasswordCorrect();
        _passwordCorrect = true;
        this->close();
    }
    else {
        qDebug()<<"password incorrect"<<endl;
        emit PassWordIncorrect();
        _passwordCorrect = false;

        QMessageBox::information(NULL, "密码错误", "请输入正确的密码");
    }
}

void KLogin::on_pushButton_2_clicked()
{
    this->close();
}
