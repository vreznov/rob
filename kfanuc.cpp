#include "kfanuc.h"
#include "m_macro.h"
#include "mg.h"

KFanuc::KFanuc(QObject *parent) : QObject(parent)
{
    KInit();
}

KFanuc::~KFanuc()
{
    if(fwLib)
    {
        close(); //关闭机床连接
        fwLib->unload();
        delete fwLib;
    }
}

void KFanuc::KInit()
{
    fwLib = new QLibrary("Fwlib32.dll", this);
    if(fwLib)
    {
//        cnnt(); //连接到机床
//        read_sys_info(); //读取机床信息

//        read_all(); //获取所有指定的信息
    }
    else
    {
        mc<<"load Fwlib32.dll error,quit"<<me;
    }
}

int KFanuc::Connect2Device()
{
    m_returnOfFw = cnc_allclibhndl3(_ip.toStdString().c_str(),
                                     _port,
                                     _tmout,
                                     &_fhandle);
    mc<<"connect result: "<<m_returnOfFw<<me;
    return m_returnOfFw;
}

void KFanuc::ReadMachineSystemInfo()
{
    short ret = cnc_sysinfo_ex(_fhandle, &_macInfo);
    ret = cnc_rdnspdl(_fhandle, &m_spinldNum);
    Q_UNUSED(ret)
}

void KFanuc::Req()
{
    this->ReadAllInfo();
    emit sig_mac_recvied();
}

// 注意。没有正确获取到机床信息不能启动查询，否则会导致程序出错退出
int KFanuc::ReadAllInfo()
{
    if(_macInfo.ctrl_axis == 0)
        return -1;

    QTime tm;
    tm.start();

    int ret = 0;
    short axis_num = _macInfo.ctrl_axis;
    short spdl_num = _macInfo.ctrl_spdl;

    //读取机床基本状态
    ODBST status = {};
    ret = cnc_statinfo(_fhandle, &status);
    m_macAuto = status.aut == 1;
    m_macRun = status.run == 3;
    m_macAlarm = status.alarm == 1;
    m_macEmergency = status.emergency == 1;

    //读取轴位置
    short pos_num = 4;
    ODBAXDT pos[pos_num*MAX_AXIS] = {};
    short pos_types[pos_num] = {0, 1, 2, 3};
    short pos_len = MAX_AXIS;
    ret = cnc_rdaxisdata(_fhandle, 1, pos_types, pos_num, &pos_len, pos);

    //伺服数据读取
    short servo_num = 3;
    ODBAXDT servo[servo_num*MAX_AXIS] = {};
    short servo_type[servo_num] = {0, 1, 2};
    short servo_len = MAX_AXIS;
    ret = cnc_rdaxisdata(_fhandle, 2, servo_type, servo_num, &servo_len, servo);

    //主轴数据读取
    short spindle_num = 4;
    ODBAXDT spindle[spindle_num*MAX_SPINDLE] = {};
    short spindle_type[spindle_num] = {0, 1, 2, 3};
    short spindle_len = MAX_SPINDLE;
    ret = cnc_rdaxisdata(_fhandle, 3, spindle_type, spindle_num, &spindle_len, spindle);

    //速度数据 屏蔽的部分无效
//    short servoSpeed_num = 3;
//    ODBAXDT servoSpeed[servoSpeed_num*MAX_AXIS] = {};
//    short servoSpeed_type[servoSpeed_num] = {0, 1, 2};
//    short servoSpeed_len = MAX_AXIS;
//    ret = cnc_rdaxisdata(_fhandle, 5, servoSpeed_type, servoSpeed_num, &servoSpeed_len, servoSpeed);
    //读取各轴转速
    long srvSpeed[100] = {0}; //注意 不能是MAX_AXIS 会出错
    ret = cnc_rdsrvspeed(_fhandle, srvSpeed);

    //程序数据  轴数量不可以填写MAX_AXIS
    odbdy prg_info = {};
    ret = cnc_rddynamic( _fhandle, 1, sizeof(prg_info), &prg_info ) ;

    //进给轴进给率/主轴速度
//    ODBACT axes_vel;
//    ODBACT spdl_vel;
//    ret = cnc_actf(_fhandle, &axes_vel);
//    ret = cnc_acts(_fhandle, &spdl_vel);

    // 读取报警信息
//    short alm_num = MAX_ALM; //读取10条报警信息
//    ODBALMMSG2 alm_msg2[MAX_ALM];
//    cnc_rdalmmsg2(m_fhandle, -1, &alm_num, alm_msg2); //para2: -1=all type.
//    for(int i=0;i<MAX_ALM;i++)
//    {
//        m_mac_alm[i] = QString(alm_msg2[i].alm_msg);
//    }

    // 集中赋值处理
    // 轴信息
    for(int i=0;i<_macInfo.ctrl_axis;i++)
    {
        m_axis_abs_pos[i] = pos[i].data / mg_pow10(pos[i].dec);
        m_axis_mac_pos[i] = pos[MAX_AXIS + i].data / mg_pow10(pos[MAX_AXIS + i].dec);
        m_axis_rel_pos[i] = pos[2 * MAX_AXIS + i].data / mg_pow10(pos[2 * MAX_AXIS + i].dec);
        m_axis_distanceToGo[i] = pos[3 * MAX_AXIS + i].data / mg_pow10(pos[3 * MAX_AXIS + i].dec);

        m_axis_load[i] = servo[i].data / mg_pow10(servo[i].dec);
        m_axis_crrnt[i] = servo[MAX_AXIS + i].data / mg_pow10(servo[MAX_AXIS + i].dec);

        m_axis_speed[i] = srvSpeed[i];
    }
    m_axis_feed_rate = prg_info.actf; //进给率
    // 主轴信息
    for(int i=0;i<_macInfo.ctrl_spdl;i++)
    {
        m_spdl_load[i] = spindle[i].data / mg_pow10(spindle[i].dec);
        //第三个数据是主轴速度
        m_spdl_speed[i] = spindle[MAX_AXIS * 3 + i].data / mg_pow10(spindle[MAX_AXIS + i].dec);
    }

    m_fanucTimestamp = DataPoint();
    m_readSpendTime = tm.elapsed();
    m_currentProgNo = prg_info.prgnum;
    m_mainPaogNo = prg_info.prgmnum;

    if(m_macAuto && m_macRun) emit sig_mac_run(); //对外通知开始读写数据
    else emit sig_mac_stop();

    return m_readSpendTime;
}

long KFanuc::ReadCurrentProgLine()
{
    ODBSEQ seqnum;
    short ret = cnc_rdseqnum(_fhandle, &seqnum);
    QString sinfo = ret!=0? "failed":"sucess";
    mc<<sinfo<<me;
    mc<<"line number "<<seqnum.data<<me;

    // 读取块号
    PRGPNT numb;
    PRGPNT pnext;
    short ret2 = cnc_rdexecpt(_fhandle, &numb, &pnext);
    mc<<"prog no."<<numb.prog_no<<"block no."<<numb.blk_no<<me;
    mc<<"next-> prog no."<<pnext.prog_no<<"block no."<<pnext.blk_no<<me;
    return seqnum.data;
}

bool KFanuc::isContected()
{
    return m_returnOfFw == EW_OK;
}

void KFanuc::close()
{
    if(_fhandle)
    {
        cnc_freelibhndl(_fhandle);
    }
}
