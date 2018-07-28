#include "mysql_opera.h"
#include "yh_ai16.h"
#include "fanuc.h"
#include "s7_reader.h"
#include <QTime>

/* 一些基本的mysql语句
 * select version() --显示版本
 * UPDATE 表名称 SET 列名称 = 新值 WHERE 列名称 = 某值 --修改列值
 * select * from 表名  --查询表内的所有数据
 * delete from 表明 where col_name = 'xxxx'   --删除符合列条件的行
 *
 *
 * */

#include "m_macro.h"
mysql_opera::mysql_opera(QObject *parent) : QObject(parent)
{
    mInit();
}

mysql_opera::~mysql_opera()
{
    if(m_cview)
    {
        m_cview->close();
        m_chart.close();
//        delete m_cview;
    }
}

void mysql_opera::mInit()
{
    sql_init();
    view_init();
}

void mysql_opera::sql_init()
{
    if(m_db.isOpen()) return;

    m_db = QSqlDatabase::addDatabase("QMYSQL");
    m_db.setHostName("localhost");
    m_db.setDatabaseName("nw_db");
    m_db.setUserName("nw1");
    m_db.setPassword("nw1");

}

void mysql_opera::get_equ_pointer(s7_reader *fml_s7, yh_ai16 *fml_yh, fanuc *fml_fa)
{
    m_s7r = fml_s7;
    m_yh = fml_yh;
    m_fa = fml_fa;

    // 连接信号槽
    connect(m_yh, &yh_ai16::sig_data_ready, this, &mysql_opera::slt_yh_trigged);
}

bool mysql_opera::sql_cnt()
{
    if(m_db.isOpen()) return true;

    bool ok = m_db.open();
    if(ok)
    {
        mc<<"connect to mysql sucessful"<<me;
    }
    else
    {
        mc<<"connect to mysql failed"<<me;
        QSqlError serr = m_db.lastError();
        QMessageBox::information(NULL,
                                 "连接数据库失败",
                                 serr.text());
    }
    get_tables_count();
    mov_mem2disk();

    QSqlQuery query;
    bool ok_req = query.exec("select version()");
    if(ok_req)
    {
        QSqlRecord srec = query.record();
        mc<<srec.value(0).toString()<<me;
    }

    m_qsl_tables = m_db.tables();

    req_mem();

    return ok;
}

void mysql_opera::close()
{
    if(m_db.isOpen())
    {
        m_db.close();
    }
}

bool mysql_opera::isOpen()
{
    return m_db.isOpen();
}

/* req_mem 查询nw_mem表的数据
 * <fml_iLLmt> 行数下限值
 * <fml_iHLmt> 行数上限值
 * */
bool mysql_opera::req_mem(uint fml_iLLmt, uint fml_iHLmt)
{
    if(m_db.isOpen())
    {
        QSqlQuery sqe("select * from nw_mem");
        sqe.exec();

        while (sqe.next()) {
            QString sinfo = sqe.value(1).toString();
            mc<<sinfo.toStdString().c_str()<<me;
        }

        return true;
    }
    else
    {
        return false;
    }
}

bool mysql_opera::req_his(uint fml_iLLmt, uint fml_iHLmt)
{
    return false;
}

bool mysql_opera::mv_mem2his()
{
    return false;
}

/* 分别操作数据库内的内存表迁移到对应的硬盘表 */
bool mysql_opera::mov_mem2disk()
{
    if(m_db.isOpen())
    {
        QSqlQuery sqe("select * from nw_mem_yh;");
        sqe.exec();

        while (sqe.next()) {
            QString sinfo = sqe.value(1).toString();
            mc<<sinfo<<me;
        }

        return true;
    }

    return false;
}

void mysql_opera::commit()
{
    if(m_db.open())
    {
        QSqlQuery sqe("commit");
    }
}

void mysql_opera::rollback()
{
    if(m_db.open())
    {
        QSqlQuery sqe("rollback");
    }
}

void mysql_opera::get_tables_count()
{
    if(m_db.open())
    {
        QSqlQuery sqe;
        // 查询采集卡表
        sqe.exec("select count(*) from nw_mem_yh");
        sqe.first();
        m_mem_yh_ct = sqe.value(0).toUInt();
        sqe.exec("select count(*) from nw_disk_yh");
        sqe.first();
        m_disk_yh_ct = sqe.value(0).toUInt();

        // 机床数据表
        sqe.exec("select count(*) from nw_mem_mac");
        sqe.first();
        m_mem_mac_ct = sqe.value(0).toUInt();
        sqe.exec("select count(*) from nw_disk_mac");
        sqe.first();
        m_disk_mac_ct = sqe.value(0).toUInt();

        // PLC表
        sqe.exec("select count(*) from nw_mem_plc");
        sqe.first();
        m_mem_plc_ct = sqe.value(0).toUInt();
        sqe.exec("select count(*) from nw_disk_plc");
        sqe.first();
        m_disk_plc_ct = sqe.value(0).toUInt();
    }
}

void mysql_opera::view_init()
{
    m_chart.legend()->hide();
    m_chart.addSeries(&m_ls);
    m_chart.createDefaultAxes();  // 基于已添加到图表的 series 来创轴
    m_chart.setTitle("研华采集数据");  // 设置图表的标题
    m_chart.axisX()->setRange(0,1000);
    m_chart.axisY()->setRange(-10, 10);
    m_ls.attachAxis(m_chart.axisY());

    m_cview = new QChartView(&m_chart);
    m_cview->setRenderHint(QPainter::Antialiasing);
    m_cview->setWindowTitle("Simple line chart");
    m_cview->resize(800, 600);

    m_cview->show();
}

// 采集卡data_ready触发
// 在此对数据进行插入到数据库的操作
void mysql_opera::slt_yh_trigged(uint fml_uiRcvCt)
{
    // 插入震动数据
    if(m_yh)
    {
        double dat[m_yh->get_channels_used_count()];
        const double* yh_data = m_yh->get_m_data();
        uint ct_per_ch = fml_uiRcvCt / m_yh->get_channels_used_count(); //每通道的数据量
        const ushort use_ct = m_yh->get_channels_used_count();
        m_ls.clear();

        QTime tm;
        tm.start();

        m_db.transaction();

        for(int i=0;i<ct_per_ch;i=i+1)
        {
            QString cmd = "insert into nw_disk_yh values(0,0,1,";
            QString c_end = ")";
            for(int j=0;j<use_ct-1;j++)
            {
                dat[j] = yh_data[i+j];
                cmd.append(QString::number(dat[j]) + ",");
            }
            cmd.append("0,0,0,0,0,0,0,0,0"); //临时措施
            cmd.append(QString::number(dat[use_ct]));
            cmd.append(c_end);

            if(m_db.isOpen())
            {
                QSqlQuery sqe(cmd);
                QSqlError err = sqe.lastError();
                int a=2;
            }
        }

        m_db.commit();
        mc<<"spand tim "<<tm.elapsed()<<me;
    }
}
