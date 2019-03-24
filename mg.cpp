#include "mg.h"

QDateTime mg_dTm; //提供全局时间，用于为数据加上时间戳
QString mg_appPath;
QString mg_dataStoragePath;

void mg_init()
{
    mg_dTm = QDateTime::currentDateTime();
    mg_appPath = QCoreApplication::applicationDirPath();
    mg_dataStoragePath = mg_appPath + "/sav_datum";
    int k=0;
}

qint64 mg_CurrentSeconds()
{
    return QDateTime::currentDateTime().toSecsSinceEpoch();
}

QString mg_CurrentDateTime()
{
    return QDateTime::currentDateTime().toString("yyyy_MM_dd_hh_mm_ss");
}

//void mg_log(QString fml_sInfo, log_level level)
//{

//}

double mg_pow10(int fml_d)
{
    return pow(10, fml_d);
}
