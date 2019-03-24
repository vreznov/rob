#ifndef KFWRITER_H
#define KFWRITER_H
/********************************************
 * 文件名：kfwriter.h
 * 作者：Kare
 * 版本: 1.0
 * 功能：对采集的数据进行记录
 ********************************************/
#include <QObject>
#include <QString>
#include <QFile>
#include <QTextStream>
#include <QTcpSocket>

class KYh_ai16;
class KFanuc;
class KS7Reader;

class KFwriter : public QObject
{
    Q_OBJECT
public:
    explicit KFwriter(QObject *parent = nullptr);
    ~KFwriter();
    void close();

private:
    void mInit();

private:
    QFile _fh_vibra; //加速度数据记录
    QFile _fh_vibraDs; //加速度精简数据
    QFile _fh_mac; //机床数据/PLC数据记录
    QTcpSocket _client; //与数据处理服务器连接。现在由matlab作为服务器进行数据处理

    KS7Reader* m_s7r = nullptr;
    KYh_ai16* m_yh = nullptr;
    KFanuc* m_fanuc = nullptr;

public:
    void get_equ_pointer(KS7Reader* fml_s7, KYh_ai16* fml_yh, KFanuc* fml_fa); //获取设备指针
    bool m_recored = false; //是否记录
    bool m_binWrite = false; //是否使用二进制文件
signals:

public slots:
    //! 采集卡数据就绪，进行保存
    //! @param fml_uiRcvCt: 总共接收的数据数量
    void slt_yh_trigged(uint fml_uiRcvCt);

    //! 机床接收到了新的数据
    void slt_mac_recvied(); //接收到了机床数据和PLC数据

    //! 创建新文件
    bool CreatFile(); //创建新的文件

    void tst_write();
    void CloseFile();
};

#endif // FWRITER_H
