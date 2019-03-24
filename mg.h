#ifndef MG_H
#define MG_H

// 全局参数
#include <QDateTime>
#include <QCoreApplication>
#include "m_enum.h"
#include <math.h>

extern QDateTime mg_dTm; //提供全局时间，用于为数据加上时间戳

extern void mg_init(); //全局数据初始化，在系统初始化时调用
extern qint64 mg_CurrentSeconds(); //返回自公元时间开始的秒数
extern QString mg_CurrentDateTime(); //返回当前日期时间
extern QString mg_appPath; //本程序所在位置
extern QString mg_dataStoragePath; //数据存储所在位置

//extern qint64 mg_CurrentSeconds(); //返回自公元时间开始的秒数
//extern QString mg_CurrentDateTime(); //返回当前日期时间
//extern QString mg_appPath; //本程序所在位置
//extern QString mg_dataStoragePath; //数据存储所在位置
extern double mg_pow10(int fml_d); //
//extern void mg_log(QString fml_sInfo, log_level level = log_level::debug); //log记录

#endif // MG_H
