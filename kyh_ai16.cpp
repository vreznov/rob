#include "kyh_ai16.h"
#include <QMessageBox>
#include "m_macro.h"

KYh_ai16::KYh_ai16(QObject *parent) : QObject(parent)
{
    KInit();
}

KYh_ai16::~KYh_ai16()
{
    if(_bufferAI)
    {
        // Release方法用于释放所有高速AI所申请的资源。如果高速AI正在运行，会先停止高速AI，然后释放资源。
        _bufferAI->Release();
        _bufferAI->Dispose();
    }

    delete[] _originalData; //删除数组
}

void KYh_ai16::KInit()
{
    _originalData = new double[MAX_RECEIVE_LENGTH];
    _vibraData.resize(MAX_CHANNEL); //重置为各个通道数值
    _vibraDataDownSample.resize(MAX_CHANNEL);
    for(int i=0;i<MAX_CHANNEL;i++)
    {
        _vibraData[i].resize(MAX_RECEIVE_LENGTH / MAX_CHANNEL);
    }

    _bufferAI = AdxBufferedAiCtrlCreate();

    // 获取基本信息
    _allDevices = _bufferAI->getSupportedDevices();
    int get_ct = _allDevices->getCount();
    if(get_ct == 0)
    {
        mc<<"can't find yh devices"<<me;
        QMessageBox::information(NULL
                                 , "读取信息错误："
                                 , "检测不到有效的采集卡"
                                 );
        return;
    }

    for(int i=0;i<get_ct;i++)
    {
        DeviceTreeNode const & node = _allDevices->getItem(i);
        m_devices.append(QString::fromWCharArray(node.Description));
    }

    _datareadyListener.owner = this; //传递自己
    _bufferAI->setStreaming(true); //设置为连续采集
    _bufferAI->addDataReadyListener(this->_datareadyListener); //设置接收事件

    SendParam2Hardware(0);
}

// 选择设备并初始化
int KYh_ai16::SendParam2Hardware(short fml_sDeviceIndex)
{
    // 如果设备在运行则停止设备
    if(_bufferAI->getState() == ControlState::Running)
    {
        mc<<"设置参数，设备停止"<<me;
        Stop();
    }

    // 设置选择的设备
    int count = _allDevices->getCount();
    m_select_device = fml_sDeviceIndex;
    if(m_select_device < 0) return -2;
    if(m_select_device > count)
    {
        mc<<"设备不存在，默认选择了设备0"<<me;
    }
    else
    {
        DeviceInformation d_info;
        d_info.DeviceNumber = m_select_device;
        _bufferAI->setSelectedDevice(d_info);
        mc<<"set device: "<<m_devices.at((m_select_device))<<me;
    }


    // 设置各个通道规格
    AiChannelCollection* aicol = _bufferAI->getChannels();
    _maxChannel = _bufferAI->getChannelCount();
    _usedChannel = _usedChannel>_maxChannel ? _maxChannel : _usedChannel; //更正一下能选择的最大通道
    // 模拟设备只有8个通道
    for(short i=0;i<_maxChannel;i++)
    {
        AnalogInputChannel& aic = aicol->getItem(i);
        aic.setValueRange(ValueRange::V_Neg5To5); //+-5V
        aic.setSignalType(AiSignalType::SingleEnded); //单端信号
    }

    // 设置转换
    ConvertClock *cc = _bufferAI->getConvertClock();
    cc->setRate(_freqency); //设置采样率，决定每秒的采集速度
    cc->setSource(SigInternalClock); //使用内建时钟，默认最高频率

    // 设置选择的通道
    ScanChannel *sch = _bufferAI->getScanChannel();
    if(sch)
    {
        sch->setChannelStart(0);
        sch->setChannelCount(_usedChannel);
        sch->setIntervalCount(_freqency/_updateFreqency); //每x组数据触发一次DataReady
        sch->setSamples(200000); //设置一个比较大的不会触发的值
    }

    //设置数据存储区大小
    for(int i=0;i<MAX_CHANNEL;i++)
    {
        _vibraData[i].resize(_freqency / _updateFreqency);
    }
    return _bufferAI->Prepare(); //设置数据到设备
}

void KYh_ai16::SetGatherParameter(ushort fml_usUseChl, uint fml_iFreq, uint fml_iUpFreq)
{
    _usedChannel = fml_usUseChl;
    _freqency = fml_iFreq;
    _updateFreqency = fml_iUpFreq;
}

void KYh_ai16::SetAmplificationFactor(ushort fml_uFac)
{
    _amplificationFactor = fml_uFac;
}

bool KYh_ai16::IsRunning()
{
    return _bufferAI->getState() == ControlState::Running;
}

ushort KYh_ai16::GetUsedChannel()
{
    return _usedChannel;
}

uint KYh_ai16::GetCounterOfChannel()
{
    return _counterOfPerChennel;
}

uint KYh_ai16::GetCounterOfDownSampleChannel()
{
    return _counterOfDownSample;
}

ushort KYh_ai16::ReqMaxChannel()
{
    AiChannelCollection* aicol = _bufferAI->getChannels();
    Q_UNUSED(aicol)
    _maxChannel = _bufferAI->getChannelCount();
    _usedChannel = _usedChannel>_maxChannel ? _maxChannel : _usedChannel; //更正一下能选择的最大通道
    return _maxChannel;
}

void KYh_ai16::Start()
{
    _bufferAI->Start();
}

void KYh_ai16::Stop()
{
    _bufferAI->Stop();
}

const QVector<QVector<DataPoint> > *KYh_ai16::GetOriginalData()
{
    return &_vibraData;
}

const QVector<QVector<DataPoint> > *KYh_ai16::GetDownSamplingData()
{
    return &_vibraDataDownSample;
}

QVector<double> KYh_ai16::DownSampling(const QVector<double>& fml_d, uint fml_M)
{
    _counterOfDownSample = fml_d.size() / fml_M;
    QVector<double> mvec(_counterOfDownSample);
    for(uint i=0;i<_counterOfDownSample;i++)
    {
        mvec[i] = fml_d[i * fml_M];
    }

    return mvec;
}

QVector<DataPoint> KYh_ai16::DownSampling(const QVector<DataPoint> &fml_d, uint fml_M)
{
    _counterOfDownSample = fml_d.size() / fml_M;
    QVector<DataPoint> mvec(_counterOfDownSample);
    for(uint i=0;i<_counterOfDownSample;i++)
    {
        mvec[i] = DataPoint(fml_d[i * fml_M].dat, fml_d[i * fml_M].timestamp);
    }

    return mvec;
}

void KYh_ai16::ConvertData(ushort fml_uiRcvCt)
{
    QTime tm;
    tm.start();
//    _counterOfPerChennel = fml_uiRcvCt / _usedChannel; //每通道的数据量
    _counterOfPerChennel = _freqency / _updateFreqency;
    QDateTime nowDTime = QDateTime::currentDateTime();
    qint64 timestamp = nowDTime.toMSecsSinceEpoch(); //当前时间转换为时间戳
    for(uint i=0;i<_counterOfPerChennel;i++)
    {
        //因为传感器物理安装位置及传感器角度与定义角度不一致，需要转换
        //对数据调换位置及根据设置参数进行放大
        int st_index = i * _usedChannel;
        double rate = 9.8/0.2 * _amplificationFactor; // 9.8/0.2=49
        if (_usedChannel >0) _convertedData[0] = _originalData[st_index + 0] * -1  * rate;
        if (_usedChannel >1) _convertedData[1] = _originalData[st_index + 1] * -1 * rate;
        if (_usedChannel >2) _convertedData[2] = _originalData[st_index + 2] * 1 * rate;

        if (_usedChannel >3) _convertedData[3] = _originalData[st_index + 4] * -1 * rate;
        if (_usedChannel >4) _convertedData[4] = _originalData[st_index + 5] * -1 * rate;
        if (_usedChannel >5) _convertedData[5] = _originalData[st_index + 3] * 1 * rate;

        if (_usedChannel >6) _convertedData[6] = _originalData[st_index + 7] * 1 * rate;
        if (_usedChannel >7) _convertedData[7] = _originalData[st_index + 6] * -1 * rate;
        if (_usedChannel >8) _convertedData[8] = _originalData[st_index + 8] * 1 * rate;

        if (_usedChannel >9) _convertedData[9] = _originalData[st_index + 9] * -1 * rate;
        if (_usedChannel >10) _convertedData[10] = _originalData[st_index + 11] * -1 * rate;
        if (_usedChannel >11) _convertedData[11] = _originalData[st_index + 10] * -1 * rate;

        if (_usedChannel >12)  _convertedData[12] = _originalData[st_index + 13] * -1 * rate;
        if (_usedChannel >13) _convertedData[13] = _originalData[st_index + 14] * 1 * rate;
        if (_usedChannel >14) _convertedData[14] = _originalData[st_index + 12] * -1 * rate;

        if (_usedChannel >15) _convertedData[15] = _originalData[st_index + 15];

        //将数据存入缓存池 queue结构
//        for(int j=0;j<MAX_CHANNEL;j++)
//        {
//            _vibraDataPoll[j].enqueue(_convertedData[j]);
//            _vibraDataPoll[j].enqueue(j); //[test]测试使用
//        }
        for(int j=0;j<MAX_CHANNEL;j++)
        {
            /*当前数据时间戳计算方法
             * _freqency/_updateFreqency  采集频率/上传频率=dataReady每通道数据量
             * 数据点时间间隔=1000/M/数据量
             * 数据时间=timestamp-(数据量-i)*时间间隔
             */
            double timeIntervel = 1000.0 / _updateFreqency / _counterOfPerChennel;
            qint64 curDTime = timestamp - (_counterOfPerChennel - i) * timeIntervel;
            DataPoint yhPoint(_convertedData[j], curDTime);
            _vibraData[j][i] = yhPoint;
        }
    }
    //对每个通道的数据进行降采样
    for(int i=0;i<MAX_CHANNEL;i++)
    {
        mc<<"i="<<i<<endl;
        _vibraDataDownSample[i] = DownSampling(_vibraData[i], 10);
        mc<<"2i="<<i<<endl;
    }

    mc<<"convert spend time "<<tm.elapsed()<<endl;
}

void KYh_ai16::slt_data_ready(void *sender, BfdAiEventArgs *args)
{
//    mc<<"recived data: "<<args->Count<<me;
    if(args->Count > MAX_GAT_PER_SEC)
    {
//        mc<<"采集的数量超出了最大缓存，请把数据限制在204800个数据"<<me;
        mc<<"yh buffer is lager then max count, clear it"<<me;
        _bufferAI->Cleanup();
        return;
    }
    else
    {
        mc<<"data_ready count is "<<args->Count<<endl;
    }

    BufferedAiCtrl* bfa = (BufferedAiCtrl*)sender;
    // 先清除数据再写入
    memset(_originalData, 0, MAX_RECEIVE_LENGTH);
    bfa->GetData(args->Count, this->_originalData); //从采集卡获取缓存数据

    m_receivedCount = args->Count;
    ConvertData(m_receivedCount); //对数据进行修正及由数字信号转换为物理单位

    emit sig_data_ready(args->Count); //对外发射接收信号
}
