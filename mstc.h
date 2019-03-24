#ifndef MSTC_H
#define MSTC_H
//定义项目用到的结构体
#include <QDateTime>

//! 一个带时间戳的数据
struct DataPoint
{
    DataPoint()
    {
        dat = 0.0;
        dtime = QDateTime::currentDateTime();
        timestamp = dtime.toMSecsSinceEpoch();
    }
    DataPoint(double fml_d, qint64 fml_timestamp)
    {
        dat = fml_d;
        timestamp = fml_timestamp;
        dtime = QDateTime::fromMSecsSinceEpoch(timestamp);
    }
    double dat; //数据
    qint64 timestamp; //时间戳
    QDateTime dtime; //日期时间
};
#endif // MSTC_H
