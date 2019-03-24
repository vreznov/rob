#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include "kfanuc.h"
#include "kyh_ai16.h"
#include "ks7reader.h"
#include "kfwriter.h"
#include "data_show.h"
#include "kmatcacu.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    void SetBackcolor(QWidget* fml_wd, Qt::GlobalColor color = Qt::gray);

private slots:

    void on_pb_yh_st_clicked(); //研华采集卡设置参数

    void on_pb_yh_stp_clicked(); //研华采集卡停止采集

    void on_pb_fCnt_clicked(); //fanuc连接

    void on_pb_fDisCnr_clicked();

    void on_pb_fRd_info_clicked(); //读取机床信息

    void on_pb_fRd_all_clicked(); //读取所有数据

    void on_pb_p_cnt_clicked();

    void on_pb_p_disCnt_clicked();

    void slt_tmr_ui_timeout();
    void on_pb_gat_start_clicked(); //采集开始
    void on_pb_gat_stop_clicked(); //采集结束
    void on_cm_amp_currentIndexChanged(const QString &arg1);
    void on_pb_tst_mac_clicked();

    void on_checkBox_stateChanged(int arg1); //是否执行记录
    void slt_mac_run(); //机床开始运行
    void slt_mac_stop(); //机床停止运行

    void on_pb_tst_crt_file_clicked();

    void on_pb_tst_close_file_clicked();

    void on_pb_rd_progline_clicked();

    //init matlab engine
    void on_pushButton_clicked();

    //stop engine
    void on_pushButton_2_clicked();

    //draw test sin(x)
    void on_pushButton_3_clicked();

    void on_pushButton_4_clicked();

private:
    Ui::MainWindow *ui;
    void KInit();
    QTimer m_tmr_ui; //用于更新界面的定时器
    data_show m_wndDatShow;
    QTime _tmGather; //已采集数据的时间

    KFanuc m_fanuc;
    KYh_ai16 m_yh;
    KS7Reader m_s7r;
    KFwriter m_fWriter;

    KMatCacu _cacu; //matlab计算

    bool m_bRec = false; //是否记录文件
};

#endif // MAINWINDOW_H
