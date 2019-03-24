#include "kfwriter.h"
#include "kyh_ai16.h"
#include "kfanuc.h"
#include "ks7reader.h"
#include "mg.h"
#include <QTime>
#include <QTextCodec>


KFwriter::KFwriter(QObject *parent) : QObject(parent)
{
    mInit();
}

KFwriter::~KFwriter()
{
    close();
}

void KFwriter::close()
{
    if(_fh_vibra.isOpen()) _fh_vibra.close();
    if(_fh_mac.isOpen()) _fh_mac.close();
    if(_fh_vibraDs.isOpen()) _fh_vibraDs.close();
}

void KFwriter::mInit()
{
}

bool KFwriter::CreatFile()
{
    bool ok1, ok2;
    QString file_vibra = mg_dataStoragePath + "/vibra_" + mg_CurrentDateTime() + ".csv";
    _fh_vibra.setFileName(file_vibra);
    ok1 = _fh_vibra.open(QIODevice::Append);

    QString file_mac =  mg_dataStoragePath + "/mac_" + mg_CurrentDateTime() + ".csv";
    _fh_mac.setFileName(file_mac);
    ok2 = _fh_mac.open(QIODevice::Append);


//    QString file_vibraDs = mg_dataStoragePath + "/vibra_ds" + mg_CurrentDateTime() + ".csv";
//    _fh_vibra.setFileName(file_vibraDs);
//    ok3 = _fh_vibraDs.open(QIODevice::Append);

    return ok1&ok2;
}

void KFwriter::tst_write()
{
//    CreatFile();

//    if(_fh_vibra.isOpen())
//    {
//        QTextStream ts(&_fh_vibra);
//        ts<<"hello, puppy"<<endl<<"write,done"<<endl;
//    }

//    _fh_vibra.close();
}

void KFwriter::CloseFile()
{
    close();
}

void KFwriter::get_equ_pointer(KS7Reader *fml_s7, KYh_ai16 *fml_yh, KFanuc *fml_fa)
{
    m_s7r = fml_s7;
    m_yh = fml_yh;
    m_fanuc = fml_fa;

    // 连接信号槽
    connect(m_yh, &KYh_ai16::sig_data_ready, this, &KFwriter::slt_yh_trigged);
    connect(m_fanuc, &KFanuc::sig_mac_recvied, this, &KFwriter::slt_mac_recvied);
}

void KFwriter::slt_yh_trigged(uint fml_uiRcvCt)
{
    Q_UNUSED(fml_uiRcvCt)
    if(!_fh_vibra.isOpen()) return;
    if(!m_recored) return;

    QTextStream ts(&_fh_vibra); //创建新的文本流
//    QTextStream tsDs(&_fh_vibraDs); //精简数据记录
    QTime tm;
    tm.start();

    const PVEC2D_YH yh_data = m_yh->GetOriginalData(); //获取未精简数据
    const PVEC2D_YH yh_dsData = m_yh->GetDownSamplingData(); //获取精简后的数据
    int ct_per_ch = m_yh->GetCounterOfChannel(); //每通道的数据量
    const ushort use_ct = m_yh->GetUsedChannel();

    //  写入数据
//    for(int i=0;i<use_ct;i++)
//    {
//        // 写入时间
//        for(int j=0;j<ct_per_ch;j++)
//        {
//            ts<<yh_data->at(i).at(j).dtime.toString("yyyy-MM-dd-hh:mm:ss:zzz") \
//             <<",channel"\
//             <<i<<","<<yh_data->at(i).at(j).dat;
//            ts<<endl; //写入换行
//        }
//    }
    //存储所有数据
    for(int j=0;j<ct_per_ch;j++)
    {
        double cols[MAX_CHANNEL + 1];
        // 写入时间
        if(!m_binWrite)
            ts<<yh_data->at(0).at(j).dtime.toString("yyyy-MM-dd-hh:mm:ss:zzz");
        else {
            cols[0] = yh_data->at(0).at(j).dtime.toMSecsSinceEpoch();
        }
        for(int i=0;i<use_ct;i++)
        {
            if(!m_binWrite && i<15) //[mark] 只需要15列数据即可
                ts<<","<<yh_data->at(i).at(j).dat;
            else {
                cols[i + 1] = yh_data->at(i).at(j).dat;
            }
        }

        if(!m_binWrite)
            ts<<endl; //写入换行
        else { //统一将数据写入文件
            const char* pDat = (char*)cols;
            _fh_vibra.write(pDat, 8 * (MAX_CHANNEL + 1));
        }
    }

    //存储精简数据 精简数据暂时不存储，直接通过socket发送
    double* pDsData = new double[(yh_dsData[0].length() + 1) * MAX_CHANNEL] () ;
    for(int j=0;j<yh_dsData[0].length(); j++)
    {
        pDsData[MAX_CHANNEL * j] = yh_dsData->at(0).at(j).dtime.toMSecsSinceEpoch();
        for (int i=0; i<yh_dsData->length(); i++)
        {
            if(!m_binWrite){
                pDsData[MAX_CHANNEL * j + i] = yh_dsData->at(i).at(j).dat;
            }
        }
    }

    _client.write((char*)pDsData, (yh_dsData[0].length() + 1) * MAX_CHANNEL * 8);
    delete[] pDsData;

    int spendTime = tm.elapsed();
    mc<<"vibra write cost time: "<<spendTime<<me;
}

// 对数控信息和PLC数据进行写入
void KFwriter::slt_mac_recvied()
{
    if(!_fh_mac.isOpen()) return;
    if(!m_recored) return;

    QTextStream ts(&_fh_mac);
    ts.setCodec(QTextCodec::codecForName("GBK"));
    QTime tm;
    tm.start();

    // 对机床写入数
    ts<<m_fanuc->m_fanucTimestamp.dtime.toString("yyyy-MM-dd-hh:mm:ss:zzz")<<"," \
    <<"spdl_speed"<<m_fanuc->m_spdl_speed[0]<<"," \
    <<"spdl_load,"<<m_fanuc->m_spdl_load[0]<<"," \

    <<"axis_feedrate,"<<m_fanuc->m_axis_feed_rate<<"," \
    <<"X_load,"<<m_fanuc->m_axis_load[0]<<"," \
    <<"Y_load,"<<m_fanuc->m_axis_load[1]<<"," \
    <<"Z_load,"<<m_fanuc->m_axis_load[2]<<"," \

    <<"X_absPos,"<<m_fanuc->m_axis_abs_pos[0]<<"," \
    <<"Y_absPos,"<<m_fanuc->m_axis_abs_pos[1]<<"," \
    <<"Z_absPos,"<<m_fanuc->m_axis_abs_pos[2]<<"," \

    <<"powup_time,"<<m_fanuc->m_mac_pwon_time<<"," \
    <<"opera_time,"<<m_fanuc->m_mac_opera_time<<"," \
    <<"cycle_time,"<<m_fanuc->m_mac_cycle_time<<"," \
    <<"cut_time,"<<m_fanuc->m_mac_cut_time<<"," \
    <<"mac_calendar,"<<m_fanuc->m_mac_calendar<<"," \
    <<"last_alarm,"<<m_fanuc->m_mac_alm[0]<<"," \
    <<"last_info,"<<m_fanuc->m_mac_info[0]<<"," \
    <<"temp1,"<<m_s7r->m_tempreture[0]<<"," \
    <<"temp2,"<<m_s7r->m_tempreture[1]<<"," \
    <<"temp3,"<<m_s7r->m_tempreture[2]<<"," \
    <<"temp4,"<<m_s7r->m_tempreture[3]<<"," \
    <<"temp5,"<<m_s7r->m_tempreture[4]<<"," \
    <<"temp6,"<<m_s7r->m_tempreture[5]<<"," \
    <<"temp7,"<<m_s7r->m_tempreture[6]<<"," \
    <<"temp8,"<<m_s7r->m_tempreture[7]<<"," \
    <<"temp9,"<<m_s7r->m_tempreture[8]<<"," \
    <<"temp10,"<<m_s7r->m_tempreture[9]<<"," \
    <<"X_current"<<m_fanuc->m_axis_crrnt[0]<<"," \
    <<"Y_current"<<m_fanuc->m_axis_crrnt[1]<<"," \
    <<"Z_current"<<m_fanuc->m_axis_crrnt[2]<<"," \
    <<"spdl_current"<<m_fanuc->m_spdl_crrnt <<endl;
    // 对PLC写入数据


    int spend_time = tm.elapsed();
    mc<<"machine write cost time: "<<spend_time<<me;
}
