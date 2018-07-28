#ifndef S7_READER_H
#define S7_READER_H

#include <QObject>
#include <QString>
#include "m_macro.h"
#include "s7_client.h"

enum class reg_type //寄存器类型
{
    mBit = 0x01,
    mByte = 0x02,
    mWord = 0x04,
    mDouble = 0x06
};

// PLC读取
class s7_reader : public QObject
{
    Q_OBJECT
public:
    explicit s7_reader(QObject *parent = nullptr);

    int cnnt(); //连接到PLC
    void dis_cnnt(); //断开连接
    void get_tempture(short (&fml_sh)[10]); //返回温度数据数组
    bool isConnected(); //返回连接状态

private:
    void mInit();
    void read_tempture(); //从PLC读取温度

    TSnap7Client clt; //PLC client对象

    qint16 m_data[10]; //接收的温度数据，10word/20bytes


    QString m_ip= "192.168.1.2";
    short m_pack = 0; //机架0
    short m_slot = 1; //槽1

signals:

public slots:
    void req(); //查询数据
};

#endif // S7_READER_H
