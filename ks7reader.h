#ifndef KS7READER_H
#define KS7READER_H

#include <QObject>
#include <QString>
#include "m_macro.h"
#include "s7_client.h"

// PLC读取
class KS7Reader : public QObject
{
    Q_OBJECT
public:
    explicit KS7Reader(QObject *parent = nullptr);

    int cnnt(); //连接到PLC
    void dis_cnnt(); //断开连接
    void GetTempture(float (&fml_sh)[MAX_TEMP]); //返回温度数据数组
    bool isConnected(); //返回连接状态
    double m_tempreture[MAX_TEMP] = {0.0};

private:
    void KInit();
    void ReadTempture(); //从PLC读取温度

    TSnap7Client clt; //PLC client对象

    qint16 m_data[MAX_TEMP]; //接收的温度数据，10word/20bytes


    QString m_ip= "192.168.1.2";
    short m_pack = 0; //机架0
    short m_slot = 1; //槽1

signals:

public slots:
    void Req(); //查询数据
};

#endif // S7_READER_H
