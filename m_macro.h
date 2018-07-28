#ifndef M_MACRO_H
#define M_MACRO_H

#include <QDebug>
#include <QMessageBox>

// 我的宏定义文件
#define mc qDebug()<<__FILE__<<" line:"<<__LINE__<<". function:"<<__FUNCTION__<<": "
#define me endl
#define MAX_GAT_PER_SEC 204800 //最大的采集存储
//#define mmsg

#endif // M_MACRO_H
