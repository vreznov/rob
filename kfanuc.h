#ifndef FANUC_H
#define FANUC_H
/********************************************
 * 文件名：kfnauc.h
 * 作者：Kare
 * 版本: 1.0
 * 功能：对纽威的NNNN型号机床进行数据访问
 * 数控系统信息
 *  厂商：FANUC
 *  型号：NNNN
 *
 * 开发中遇到的问题
 * 1.为什么使用了从DLL获取函数指针而不是直接使用FWLIB32.H
 *      错误的fwlib32.lib会导致指针错误，所以使用了从dll获取
 *     函数指针的方法
 ********************************************/
#include <QObject>
#include <QLibrary>
#include <QString>
#include <QTime>
#include "m_macro.h"
#include "kexception.h"
#include "mstc.h"
#include <Fwlib32.h>
#include <QThread>
// freelibhndl
//
class KFanuc : public QObject
{
    Q_OBJECT
public:
    explicit KFanuc(QObject *parent = nullptr);
    ~KFanuc();

    //! 返回与机床通信状态
    //! @return: true-连接/false-断开
    bool isContected();

    //! 释放句柄，关闭连接
    void close();


private:
    //! 载入Fanuc Fwlib32.lib
    QLibrary* fwLib = NULL;

    QString _ip = tr("192.168.1.1");
    ushort _port = 8193;
    long _tmout = 10; //连接超时(s)
    ushort _fhandle = 0; //fanuc handle

    void KInit(); //初始化

    //! 存储机床信息
    ODBSYSEX _macInfo;

public:
    //! 连接到机床
    //! @return:
    int Connect2Device();

    //! 读取系统信息
    void ReadMachineSystemInfo();

    //! 读取当前项目中所需要的所有信息
    //! @return:
    int ReadAllInfo();

    //! 读取机床当前正在执行的数控程序行号
    //! @return: Line number
    long ReadCurrentProgLine();

public:
    int m_readSpendTime = 0; //从机床读取所有数据所消耗的时间
    //! 连接机床函数返回结果
    short m_returnOfFw = 0xffff; //连接机床函数返回结果

public: //存储读取的结果
    // 注意数组初始化
    short m_spinldNum; //主轴数量
    bool m_macAuto = false; //机床自动状态
    bool m_macRun = false; //机床在运行状态
    bool m_macAlarm = false; //机床报警
    bool m_macEmergency = false; //机床急停
    long m_spdl_err; //主轴误差
    double m_axis_abs_pos[MAX_AXIS] = {0.0}; //轴绝对位置
    double m_axis_mac_pos[MAX_AXIS] = {0.0}; //轴机械位置
    double m_axis_rel_pos[MAX_AXIS] = {0.0}; //轴相对位置
    double m_axis_distanceToGo[MAX_AXIS] = {0.0}; //轴相对位置
    long m_axis_feed_rate = 0; //进给速率
    double m_axis_load[MAX_AXIS] = {0}; //轴负载
    long m_axis_speed[MAX_AXIS] = {0};

    double m_spdl_speed[MAX_SPINDLE] = {0.0}; //主轴速度
    double m_spdl_load[MAX_SPINDLE] = {0.0}; //主轴负载

    long m_axis_crrnt[MAX_AXIS]; //轴电流
    long m_axis_vel[MAX_AXIS]; //轴转速
    long m_spdl_crrnt = 0; //主轴电流
    long m_currentProgNo = 0; //当前执行的程序编号
    long m_mainPaogNo = 0; //主程序编号
    ushort m_reg_prg = 0; //注册程序数量
    ushort m_unreg_prg = 0; //未注册数量
    long m_used_mem = 0; //已使用存储
    long m_unused_mem; //未使用存储
    long m_mac_pwon_time = 0; //机床上电时间
    long m_mac_opera_time = 0; //机床操作时间
    long m_mac_cycle_time = 0; //机床循环时间
    long m_mac_cut_time = 0; //机床剪切时间
    QString m_mac_calendar; //机床日历
    QString m_mac_alm[MAX_ALM]; //机床报警信息
    QString m_mac_info[MAX_INFO]; //机床信息

    DataPoint m_fanucTimestamp; //发那科获取数据时间戳，以最后一次获取的数据时间为准

signals:
    void sig_mac_recvied();
    void sig_mac_run(); //机床开始运行
    void sig_mac_stop(); //机床停止运行
public slots:
    //! 查询机床数据
    void Req();
};

class KFanucThread : public QObject
{
    Q_OBJECT
public:
    KFanucThread(KFanuc* fml_pFanuc)
    {
        _pFanuc = fml_pFanuc;
        _pFanuc->moveToThread(&_thread);
        _thread.start();
        _pFanuc->Req();
    }
    ~KFanucThread()
    {
        _thread.quit();
        _thread.wait();
    }
    QThread _thread;

private:
    KFanuc* _pFanuc = nullptr;
};
#endif // FANUC_H
