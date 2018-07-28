#include "yh_ai16.h"
#include <QMessageBox>
#include "m_macro.h"

yh_ai16::yh_ai16(QObject *parent) : QObject(parent)
{
    mInit();
}

yh_ai16::~yh_ai16()
{
    if(bufferAI)
    {
        // Release方法用于释放所有高速AI所申请的资源。如果高速AI正在运行，会先停止高速AI，然后释放资源。
        bufferAI->Release();
        bufferAI->Dispose();
    }
}

void yh_ai16::mInit()
{
    bufferAI = BufferedAiCtrl::Create();

    // 获取基本信息
    supportedDevice = bufferAI->getSupportedDevices();
    int get_ct = supportedDevice->getCount();
    if(get_ct == 0)
    {
        mc<<"获取不到有效的研华采集卡设备"<<me;
        QMessageBox::information(NULL
                                 , "读取信息错误："
                                 , "检测不到有效的采集卡"
                                 );
        return;
    }

    for(int i=0;i<get_ct;i++)
    {
        DeviceTreeNode const & node = supportedDevice->getItem(i);
        m_devices.append(QString::fromWCharArray(node.Description));
    }

    m_dr_listener.owner = this; //传递自己
    bufferAI->setStreaming(true); //设置为连续采集
    bufferAI->addDataReadyListener(this->m_dr_listener); //设置接收事件

    set_hdr(0);
}

// 选择设备并初始化
void yh_ai16::set_hdr(short fml_sDeviceIndex)
{
    // 如果设备在运行则停止设备
    if(bufferAI->getState() == ControlState::Running)
    {
        mc<<"设置参数，设备停止"<<me;
        stop();
    }

    // 设置选择的设备
    m_select_device = fml_sDeviceIndex;
    if(m_select_device > supportedDevice->getCount())
    {
        mc<<"设备不存在，默认选择了设备0"<<me;
    }
    else
    {
        DeviceInformation d_info;
        d_info.DeviceNumber = m_select_device;
        bufferAI->setSelectedDevice(d_info);
        mc<<"set device: "<<m_devices.at((m_select_device))<<me;
    }


    // 设置各个通道规格
    Array<AiChannel>* aicol = bufferAI->getChannels();
    m_max_chl = bufferAI->getChannelCount();
    m_chl_used = m_chl_used>m_max_chl ? m_max_chl : m_chl_used; //更正一下能选择的最大通道
    // 模拟设备只有8个通道
    for(short i=0;i<m_max_chl;i++)
    {
        AiChannel& aic = aicol->getItem(i);
        aic.setValueRange(ValueRange::V_Neg5To5); //+-5V
        aic.setSignalType(AiSignalType::SingleEnded); //单端信号
    }

    // 设置转换
    ConvertClock *cc = bufferAI->getConvertClock();
    cc->setRate(m_rate); //设置采样率，决定每秒的采集速度
    cc->setSource(SigInternalClock); //使用内建时钟，默认最高频率

    // 设置选择的通道
    ScanChannel *sch = bufferAI->getScanChannel();
    if(sch)
    {
        sch->setChannelStart(0);
        sch->setChannelCount(m_chl_used);
        sch->setIntervalCount(m_rate/m_up_rate); //每秒触发一次DataReady
        sch->setSamples(200000); //设置一个比较大的不会触发的值
    }

    bufferAI->Prepare(); //设置数据到设备
}

void yh_ai16::set_gat_para(ushort fml_usUseChl, uint fml_iRate, uint fml_iUp_rate)
{
    m_chl_used = fml_usUseChl;
    m_rate = fml_iRate;
    m_up_rate = fml_iUp_rate;
}

bool yh_ai16::isRunning()
{
    return bufferAI->getState() == ControlState::Running;
}

const ushort yh_ai16::get_channels_used_count()
{
    return m_chl_used;
}

ushort yh_ai16::get_max_channels_ct()
{
    Array<AiChannel>* aicol = bufferAI->getChannels();
    m_max_chl = bufferAI->getChannelCount();
    m_chl_used = m_chl_used>m_max_chl ? m_max_chl : m_chl_used; //更正一下能选择的最大通道
    return m_max_chl;
}

void yh_ai16::start()
{
    bufferAI->Start();
}

void yh_ai16::stop()
{
    bufferAI->Stop();
}

const double *yh_ai16::get_m_data() const
{
    return m_data;
}

void yh_ai16::slt_data_ready(void *sender, BfdAiEventArgs *args)
{
    if(args->Count > MAX_GAT_PER_SEC)
    {
        mc<<"采集的数量超出了最大缓存，请把数据限制在204800个数据"<<me;
        return;
    }

    BufferedAiCtrl* bfa = (BufferedAiCtrl*)sender;
    // 先清除数据再写入
    memset(m_data, 0, sizeof(m_data));
    bfa->GetData(args->Count, this->m_data);

    m_cur_recive = args->Count;
    emit sig_data_ready(args->Count); //对外发射接收信号
}
