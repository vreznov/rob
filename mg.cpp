#include "mg.h"

QDateTime mg_dTm; //提供全局时间，用于为数据加上时间戳

void mg_init()
{
    mg_dTm = QDateTime::currentDateTime();
}

qint64 mg_cur_sec()
{
    return QDateTime::currentDateTime().toSecsSinceEpoch();
}
