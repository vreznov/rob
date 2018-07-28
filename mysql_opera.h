#ifndef MYSQL_OPERA_H
#define MYSQL_OPERA_H

#include <QObject>
#include <QStringList>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QThread>
#include <QSqlRecord>
#include <QSqlError>
#include "m_macro.h"
#include <QMessageBox>
#include <QChart>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>

using namespace QtCharts;

class yh_ai16;
class fanuc;
class s7_reader;
/* 提供对mysql的读写操作
 * 读取的数据默认存入内存表单，一定时间后存入其他表。
 * 注意，内存表数量有限1G
 * 内存表名称 nw_mem
 * 本地存储表名称 nw_his
 *
 * */
class mysql_opera : public QObject
{
    Q_OBJECT
public:
    explicit mysql_opera(QObject *parent = nullptr);
    ~mysql_opera();

private:
    void mInit();
    void sql_init();
    QSqlDatabase m_db; //提供数据库操作
    QStringList m_qsl_tables; //表名称们
    uint max_mem_rows = 1000000; //nw_mem最大存储行数1000,000

    s7_reader* m_s7r = nullptr;
    yh_ai16* m_yh = nullptr;
    fanuc* m_fa = nullptr;

public:
    void get_equ_pointer(s7_reader* fml_s7, yh_ai16* fml_yh, fanuc* fml_fa); //获取设备指针
    bool sql_cnt();
    void close(); //关闭数据库
    bool isOpen(); //返回数据库状态
    bool req_mem(uint fml_iLLmt=0, uint fml_iHLmt=0); //查询nw_mem表的数据
    bool req_his(uint fml_iLLmt=0, uint fml_iHLmt=0); //查询nw_his表的数据
    bool mv_mem2his(); //从内存表存储本地表
    bool insert_val(); //插入数据到数据表
    uint m_mem_yh_ct = 0;
    uint m_disk_yh_ct = 0;
    uint m_mem_mac_ct = 0;
    uint m_disk_mac_ct = 0;
    uint m_mem_plc_ct = 0;
    uint m_disk_plc_ct = 0;

private: //数据库操作
    bool mov_mem2disk(); //将内存数据库中的数据移动到硬盘数据库
    void commit(); //对数据进行提交
    void rollback(); //回滚数据
    void get_tables_count(); //获取使用的几张表的行数

private: //测试使用的chart
    QLineSeries m_ls;
    QChart m_chart;
    QChartView* m_cview;
    void view_init();
signals:

public slots:
    void slt_yh_trigged(uint fml_uiRcvCt); //采集卡数据触发
};

#endif // MYSQL_OPERA_H
