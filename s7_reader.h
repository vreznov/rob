#ifndef S7_READER_H
#define S7_READER_H

#include <QObject>
#include <QString>

#include "s7_client.h"
#include "m_macro.h"

// PLC读取
class s7_reader : public QObject
{
    Q_OBJECT
public:
    explicit s7_reader(QObject *parent = nullptr);

    int cnn(); //连接到PLC

private:
    void mInit();
    TSnap7Client clt; //PLC client对象

private:
    QString m_ip= "192.168.1.2";
    short m_pack = 0; //机架0
    short m_slot = 1; //槽1


signals:

public slots:
};

#endif // S7_READER_H
