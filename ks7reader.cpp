#include "s7_reader.h"

s7_reader::s7_reader(QObject *parent) : QObject(parent)
{
    mInit();
}

int s7_reader::cnnt()
{
    int ret = clt.ConnectTo(m_ip.toStdString().c_str(), m_pack, m_slot);

    return ret;
}

void s7_reader::dis_cnnt()
{
    if(isConnected())
    {
        clt.Disconnect();
    }
}

/* get_tempture 获取温度数据
 * <fml_sh> 要返回写入的数组，因为返回只能用指针，但是形参可以用数组并且限制大小
 * */
void s7_reader::get_tempture(short (&fml_sh)[10])
{
    // 直接拷贝即可，连续内存组成连续的数据
    memcpy(fml_sh, m_data, sizeof(fml_sh));
}

void s7_reader::mInit()
{

}

void s7_reader::req()
{
    read_tempture();
}

void s7_reader::read_tempture()
{
    if(isConnected())
    {
        // 从DB2读取10个word
        clt.DBRead(2, 0, 2*10, m_data);
    }
}

bool s7_reader::isConnected()
{
    return clt.Connected;
}

