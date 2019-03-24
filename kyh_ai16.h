#ifndef KYH_AI16_H
#define KYH_AI16_H
/********************************************
 * 文件名：kyh_ai16.h
 * 作者：Kare
 * 版本: 1.0
 * 功能：对研华采集卡的操作
 ********************************************/
#include <QObject>
#include <QList>
#include <QString>
#include <QScopedPointer>
#include <QVector>
#include "bdaqctrl.h" //研华控件文件
#include "m_macro.h"
#include "mstc.h"
using namespace Automation::BDaq;

// 研华16AI采集模块
// PCIE-1816/PCI-1716L
#include <QVector>
#include <QQueue>
#include <QDateTime>



class KYh_ai16 : public QObject
{
    Q_OBJECT
public:
    explicit KYh_ai16(QObject *parent = nullptr);
    ~KYh_ai16();

private:
    void KInit();

private: //设备硬件相关
    ICollection<DeviceTreeNode> *_allDevices = nullptr;
    ushort _usedChannel = 16; //设置使用了几个通道
    ushort _maxChannel = 16; //存储读取到的设备的最大通道数
    uint _counterOfPerChennel = 0u; //每通道采集的数据量
    uint _counterOfDownSample = 0u; //存储降采样后的每通道数据量
    static const ushort _maxFreqency = 20000; //最大采集20,000/s
    bool _streaming = true; //连续采集
    uint _freqency = 1000; //采集速度，10/s
    uint _updateFreqency = 10; //每秒上传速率，决定了数据更新的快慢
    BufferedAiCtrl *_bufferAI = nullptr;
    ushort _amplificationFactor = 1; //放大倍率 amplification factor

    // 接收data_ready的类。详细用法查看研华手册的C++示例
    // 在此回调数据处理
    class BfdAiEventHandle : public BfdAiEventListener
    {
    public:
        KYh_ai16* owner;
        virtual void BDAQCALL BfdAiEvent(void* sender ,BfdAiEventArgs* args )
        {
            owner->slt_data_ready(sender, args);
        }
    };
    BfdAiEventHandle _datareadyListener; //data_ready event reciver

public: //获取/设置硬件信息
    ushort GetUsedChannel(); //获取当前的使用通道数量
    uint GetCounterOfChannel(); //获取当前每通道的数据数量
    uint GetCounterOfDownSampleChannel(); //获取降采样后的每通道数据量
    ushort ReqMaxChannel(); //获取设备的最大通道数
    int SendParam2Hardware(short fml_sDeviceIndex); //设置硬件采集参数
    void SetGatherParameter(ushort fml_usUseChl, uint fml_iFreq, uint fml_iUpFreq); //设置采集参数，最大使用通道和采集速度
    void SetAmplificationFactor(ushort fml_uFac); //设置放大倍率
    bool IsRunning(); //是否正在运行
    short m_select_device = 0;
    QList<QString> m_devices; //所检测到的设备

public:
    void Start();  //开始采集
    void Stop(); //停止采集

    const PVEC2D_YH GetOriginalData(); //获取数据指针
    const PVEC2D_YH GetDownSamplingData(); //获取降采样后的数据
    int m_receivedCount = 0; //当前接收的数据量

    //! 整数倍降采样函数
    //! @param fml_d:原数据
    //! @param fml_M:降低采样的倍数
    QVector<double> DownSampling(const QVector<double>& fml_d, uint fml_M = 10);
    QVector<DataPoint> DownSampling(const QVector<DataPoint> &fml_d, uint fml_M = 10);
private:
    // 不要在栈上分配太多内存
    //! 从采集卡获取的原始数据
    double* _originalData;
    double _convertedData[MAX_CHANNEL]; //临时存储转换后的数据

    //! 震动数据缓存，最终保存到文件
    //! 第一维采用vector，第二维采用queue结构 先入先出
    //! 每个通道最多存储20K个数据
    QVector<QVector<DataPoint>> _vibraData;

    //! 降采样后的震动数据，用于方便在界面绘制图形
    QVector<QVector<DataPoint>> _vibraDataDownSample;

    //! 对采集的数据进行转换处理.fml_uiRcvCt 本次接收的数据数量。
    void ConvertData(ushort fml_uiRcvCt);
signals:
    //! 接收到了数据完成信号
    //! @param fml_uiCt 接收的数据数量
    void sig_data_ready(uint fml_uiCt);
public slots:
    //我的数据就绪处理函数
    void slt_data_ready(void* sender ,BfdAiEventArgs* args );
};

#endif // YH_AI16_H
