#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include "fanuc.h"
#include "yh_ai16.h"
#include "s7_reader.h"
#include "mysql_opera.h"

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
    void set_bk_color(QWidget* fml_wd, Qt::GlobalColor color = Qt::gray);

private slots:
    void on_cm_devices_currentIndexChanged(int index);

    void on_pb_db_cnt_clicked();

    void on_pb_db_dCnt_clicked();

    void on_pb_yh_st_clicked();

    void on_pb_yh_stp_clicked();

    void on_pb_fCnt_clicked();

    void on_pb_fDisCnr_clicked();

    void on_pb_fRd_info_clicked();

    void on_pb_fRd_all_clicked();

    void on_pb_p_cnt_clicked();

    void on_pb_p_disCnt_clicked();

private:
    Ui::MainWindow *ui;
    QTimer m_tmr_ui; //用于更新界面的定时器

    void mInit();

    fanuc m_fa;
    yh_ai16 m_yh;
    s7_reader m_s7r;
    mysql_opera m_sop;

private slots:
    void slt_tmr_ui_timeout();
};

#endif // MAINWINDOW_H
