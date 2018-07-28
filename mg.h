#ifndef MG_H
#define MG_H

// 全局参数
#include <QDateTime>
#include <QDialog>
extern QDateTime mg_dTm; //提供全局时间，用于为数据加上时间戳

extern void mg_init(); //全局数据初始化，在系统初始化时调用
extern qint64 mg_cur_sec(); //返回自公元时间开始的秒数

#endif // MG_H
