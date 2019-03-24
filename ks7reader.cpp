#include "ks7reader.h"

enum class reg_type //寄存器类型
{
    mBit = 0x01,
    mByte = 0x02,
    mWord = 0x04,
    mDouble = 0x06
};

KS7Reader::KS7Reader(QObject *parent) : QObject(parent)
{
    KInit();
}

int KS7Reader::cnnt()
{
    int ret = clt.ConnectTo(m_ip.toStdString().c_str(), m_pack, m_slot);

    return ret;
}

void KS7Reader::dis_cnnt()
{
    if(isConnected())
    {
        clt.Disconnect();
    }
}

/* get_tempture 获取温度数据
 * <fml_sh> 要返回写入的数组，因为返回只能用指针，但是形参可以用数组并且限制大小
 * */
void KS7Reader::GetTempture(float (&fml_sh)[MAX_TEMP])
{
    // 直接拷贝即可，连续内存组成连续的数据
//    ushort temp[MAX_TEMP] = {0.0};
//    memcpy(temp, m_data, sizeof(temp));

//    for(int i=0;i<MAX_TEMP;i++)
//    {
//        fml_sh[i] = temp[i] / 1000.0f;
//    }
}

void KS7Reader::KInit()
{

}

void KS7Reader::Req()
{
    ReadTempture();
}

void KS7Reader::ReadTempture()
{
    if(isConnected())
    {
        // 从DB2读取10个word
        memset(m_data, 0, sizeof(m_data));
        int ret = clt.DBRead(2, 0, 2*10, m_data);

        quint8 temp[MAX_TEMP*2] = {0};
        memcpy(temp, m_data, sizeof(m_data));
        //读取的数据需要进行高低位互换
        if(!ret)
        {
            for(int i=0;i<MAX_TEMP;i++)
            {
                quint16 hbyte = ((quint16)temp[i*2])<<8;
                quint16 lbyte = (quint16)temp[i*2+1];
                m_tempreture[i] = (hbyte + lbyte) / 10.0;
            }
        }
    }
}

bool KS7Reader::isConnected()
{
    return clt._connected;
}

