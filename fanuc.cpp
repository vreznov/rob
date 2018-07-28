#include "fanuc.h"
#include "m_macro.h"


fanuc::fanuc(QObject *parent) : QObject(parent)
{
    mInit();
}

fanuc::~fanuc()
{
    if(m_lib)
    {
        close(); //关闭机床连接
        m_lib->unload();
        delete m_lib;
    }
}

void fanuc::mInit()
{
    m_lib = new QLibrary("Fwlib32.dll", this);
    if(m_lib)
    {
        get_fun_pnter(); //获取指针
//        cnnt(); //连接到机床
//        read_sys_info(); //读取机床信息

//        read_all(); //获取所有指定的信息
    }
    else
    {
        mc<<"load Fwlib32.dll error,quit"<<me;
    }

    memset(m_abs_pos, 0, sizeof(m_abs_pos));
    memset(m_axis_load, 0, sizeof(m_axis_load));
    memset(m_axis_spdl_load, 0 ,sizeof(m_axis_spdl_load));
}

int fanuc::cnnt()
{
    cnt_ret = cnn(m_ip.toStdString().c_str(),
                                     m_port,
                                     m_tmout,
                                     &m_fhandle);
    mc<<"connect result: "<<cnt_ret<<me;
    return cnt_ret;
}

// 从DLL获取函数指针
// 如果有未能获取的会直接退出并返回错误代码
int fanuc::get_fun_pnter()
{
    bool ret_load = m_lib->load();
    if (ret_load)
    {
        mc<<"load sucess"<<me;
    }
    else
    {
        mc<<"load failed"<<me;
        return -1;
    }

    pcnc_sysinfo_ex = (cnc_sysinfo_ex)m_lib->resolve("cnc_sysinfo_ex");
    if(!pcnc_sysinfo_ex) return -2;

    cnn = (cnc_allclibhndl3)m_lib->resolve("cnc_allclibhndl3");
    if(!cnn) return -3;

    pcnc_absolute = (cnc_absolute)m_lib->resolve("cnc_absolute");
    if(!pcnc_absolute) return -4;

    pcnc_actf = (cnc_actf)m_lib->resolve("cnc_actf");
    if(!pcnc_actf) return -5;

    pcnc_acts = (cnc_acts)m_lib->resolve("cnc_acts");
    if(!pcnc_acts) return -6;

    pcnc_rdsvmeter = (cnc_rdsvmeter)m_lib->resolve("cnc_rdsvmeter");
    if(!pcnc_rdsvmeter) return -7;

    pcnc_rdspmeter = (cnc_rdspmeter)m_lib->resolve("cnc_rdspmeter");
    if(!pcnc_rdspmeter) return -8;

    pcnc_rdproginfo = (cnc_rdproginfo)m_lib->resolve("cnc_rdproginfo");
    if(!pcnc_rdproginfo) return -9;

    pcnc_rdcurrent = (cnc_rdcurrent)m_lib->resolve("cnc_rdcurrent");
    if(!pcnc_rdcurrent) return -10;

    pcnc_alarm = (cnc_alarm)m_lib->resolve("cnc_alarm");
    if(!pcnc_alarm) return -11;

    pcnc_rdalmmsg2 = (cnc_rdalmmsg2)m_lib->resolve("cnc_rdalmmsg2");
    if(!pcnc_rdalmmsg2) return -12;

    pcnc_freelibhndl = (cnc_freelibhndl)m_lib->resolve("cnc_freelibhndl");
    if(!pcnc_freelibhndl) return -12;

    return 0;
}

void fanuc::read_sys_info()
{
    short ret = pcnc_sysinfo_ex(m_fhandle, &m_info);
    Q_UNUSED(ret)
}

void fanuc::req()
{
    this->read_all();
}

// 注意。没有正确获取到机床信息不能启动查询，否则会导致程序出错退出
void fanuc::read_all()
{
    if(m_info.ctrl_axis == 0)
        return;

    QTime tm;
    tm.start();

    int ret = 0;
    Q_UNUSED(ret)

    short axis_num = m_info.ctrl_axis;
    short spdl_num = m_info.ctrl_spdl;

    // absolute pos
    ODBAXIS ax_da;
    ret = pcnc_absolute(m_fhandle, ALL_AXES, 4+4*MAX_AXIS, &ax_da);

    // 进给轴速度/主轴速度
    ODBACT cact_da;
    ret = pcnc_actf(m_fhandle, &cact_da);
    ret = pcnc_acts(m_fhandle, &cact_da);

    // 进给轴负载/主轴负载
    // 注意：如果控制轴数为0会出错
    ODBSVLOAD svload[m_info.ctrl_axis]; //轴负载
    ODBSPLOAD spload[m_info.ctrl_spdl]; //主轴负载
    ret = pcnc_rdsvmeter(m_fhandle, &axis_num, svload);
    ret = pcnc_rdspmeter(m_fhandle, 0, &spdl_num, spload);

    // 读取程序信息
    ODBNC prog_info;
    ret = pcnc_rdproginfo(m_fhandle, 0, 12, &prog_info); //para2:0 bin/1 ascii. para3:12 bin/31 ascii
    m_reg_prg = prog_info.u.bin.reg_prg;
    m_unreg_prg = prog_info.u.bin.unreg_prg;
    m_used_mem = prog_info.u.bin.used_mem;
    m_unused_mem = prog_info.u.bin.unused_mem;

    // 读取电流
    short crrnt[MAX_AXIS];
    memset(crrnt, 0, sizeof(crrnt));
    pcnc_rdcurrent(m_fhandle, crrnt);

    // 读取报警信息
    short alm_num = m_max_alm; //读取10条报警信息
    ODBALMMSG2 alm_msg2[m_max_alm];
    pcnc_rdalmmsg2(m_fhandle, -1, &alm_num, alm_msg2); //para2: -1=all type.
    for(int i=0;i<m_max_alm;i++)
    {
        m_alms[i] = QString(alm_msg2[i].alm_msg);
    }

    // 集中赋值处理
    // 轴信息
    for(int i=0;i<m_info.ctrl_axis;i++)
    {
        m_abs_pos[i] = ax_da.data[i];
        m_axis_load[i] = svload[i].svload.data;
    }
    // 主轴信息
    for(int i=0;i<m_info.ctrl_spdl;i++)
    {
        m_axis_spdl_load[i] = spload[i].spload.data;
    }

    rd_tm = tm.elapsed();
    mc<<"Fanuc读取消耗时间(ms)："<<rd_tm<<me;
}

bool fanuc::isCnted()
{
    return cnt_ret == EW_OK;
}

void fanuc::close()
{
    pcnc_freelibhndl(m_fhandle);
}
