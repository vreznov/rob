#ifndef YH_AI16_H
#define YH_AI16_H

#include <QObject>
#include <QList>
#include <QString>
#include "bdaqctrl.h"
#include "m_macro.h"

using namespace Automation::BDaq;

// 研华16AI采集模块
// PCIE-1816/PCI-1716L

class yh_ai16 : public QObject
{
    Q_OBJECT
public:
    explicit yh_ai16(QObject *parent = nullptr);
    ~yh_ai16();

private:
    void mInit();

private: //设备硬件相关
    Array<DeviceTreeNode> *supportedDevice = nullptr;
    ushort m_chl_used = 16; //设置使用了几个通道
    ushort m_max_chl = 16; //存储读取到的设备的最大通道数
    static const ushort m_max_rate = 20000; //最大采集20,000/s
    bool streaming = true; //连续采集
    uint m_rate = 1000; //采集速度，10/s
    uint m_up_rate = 10; //每秒上传速率，决定了数据更新的快慢
    BufferedAiCtrl *bufferAI = nullptr;

    // 接收data_ready的类
    class BfdAiEventHandle : public BfdAiEventListener
    {
    public:
        yh_ai16* owner;
        virtual void BDAQCALL BfdAiEvent(void* sender ,BfdAiEventArgs* args )
        {
            owner->slt_data_ready(sender, args);
        }
    };
     BfdAiEventHandle m_dr_listener; //data_ready event reciver

public: //获取/设置硬件信息
    const ushort get_channels_used_count(); //获取当前的使用通道数量
    ushort get_max_channels_ct(); //获取设备的最大通道数
    void set_hdr(short fml_sDeviceIndex); //设置硬件采集参数
    void set_gat_para(ushort fml_usUseChl, uint fml_iRate, uint fml_iUp_rate); //设置采集参数，最大使用通道和采集速度
    bool isRunning(); //是否正在运行
    short m_select_device = 0;
    QList<QString> m_devices; //所检测到的设备

public:
    void start();  //开始采集
    void stop(); //停止采集

    static const uint MAX_RECIVE_LENGTH = 204800; //最大的接收缓冲区
    double m_data[MAX_RECIVE_LENGTH]; //默认分配1.6MB缓存,允许存储12800行的数据，即每秒最多采集12800
    const double* get_m_data() const; //获取数据指针
    int m_cur_recive = 0; //当前接收的数据量
signals:
    // 接收到了数据完成信号
    // fml_uiCt 接收的数据数量
    void sig_data_ready(uint fml_uiCt);
public slots:
    //我的数据就绪处理函数
    void slt_data_ready(void* sender ,BfdAiEventArgs* args );
};

#endif // YH_AI16_H
