#ifndef M_MACRO_H
#define M_MACRO_H

#include <QDebug>
#include <QMessageBox>

// 我的宏定义文件
#define mc qDebug()<<__FILE__<<" line:"<<__LINE__<<". function:"<<__FUNCTION__<<": "
#define me endl
#define MAX_GAT_PER_SEC 204800 //最大的采集存储
#define MAX_ALM 30  //最多存储的报警信息
#define MAX_INFO 30 //最多存储的机床信息
#define MAX_TEMP 10 //最多存储温度信息
#define MAX_RECEIVE_LENGTH 320000 //接收缓冲区
#define MAX_CHANNEL 16 //研华采集卡最大通道数
#define DOWNSAMPLE_M 10U //降采样倍数


#define VEC_YH QVector<DataPoint>
#define VEC2D_YH QVector<VEC_YH>
#define PVEC_YH QVector<DataPoint>*
#define PVEC2D_YH QVector<VEC_YH>*
//#define mmsg

#endif // M_MACRO_H
