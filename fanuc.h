#ifndef FANUC_H
#define FANUC_H

#include <QObject>
#include <QLibrary>
#include <QString>
#include <QTime>
#include "fw_header.h"

// freelibhndl
//
class fanuc : public QObject
{
    Q_OBJECT
public:
    explicit fanuc(QObject *parent = nullptr);
    ~fanuc();

    bool isCnted(); //返回是否与机床连接
    void close(); //释放句柄，关闭连接


private:
    QLibrary* m_lib = NULL;
     QString m_ip = tr("192.168.1.1");
    ushort m_port = 8193;
    long m_tmout = 10; //连接超时(s)
    ushort m_fhandle = 0; //fanuc handle

    const static int m_max_alm = 10; //最多读取10条报警信息

    void mInit();
    int get_fun_pnter(); //从Fwlib32.dll中获取函数指针

    ODBSYSEX m_info; //machine info

public:
    int cnnt(); //l连接到机床
    void read_sys_info(); //读取系统信息
    void read_all(); //read all datum

private: //function pointer
    cnc_allclibhndl3 cnn = nullptr;
    cnc_absolute pcnc_absolute = nullptr;
    cnc_actf pcnc_actf = nullptr;
    cnc_acts pcnc_acts = nullptr;
    cnc_sysinfo_ex pcnc_sysinfo_ex = nullptr;
    cnc_rdsvmeter pcnc_rdsvmeter = nullptr;
    cnc_rdspmeter pcnc_rdspmeter = nullptr;
    cnc_rdproginfo pcnc_rdproginfo = nullptr;
    cnc_rdcurrent pcnc_rdcurrent = nullptr;
    cnc_alarm pcnc_alarm = nullptr;
    cnc_rdalmmsg2 pcnc_rdalmmsg2 = nullptr;
    cnc_freelibhndl pcnc_freelibhndl = nullptr;

public:
    int rd_tm = 0; //读取所消耗的时间
    short cnt_ret = 0xffff; //连接机床函数返回结果

public: //存储读取的结果
    long m_abs_pos[MAX_AXIS];
    long m_axis_feed_rate = 0; //进给速度
    long m_spld_speed = 0; //主轴速度
    long m_axis_load[MAX_AXIS]; //轴负载
    long m_axis_spdl_load[MAX_SPINDLE]; //主轴负载
    long m_axis_crrnt[MAX_AXIS]; //轴电流
    short m_reg_prg = 0; //注册程序数量
    short m_unreg_prg = 0; //未注册数量
    long m_used_mem = 0; //已使用存储
    long m_unused_mem; //未使用存储
    QString m_alms[m_max_alm]; //存储N条报警信息

signals:

public slots:
    void req(); //查询机床数据
};

#endif // FANUC_H
