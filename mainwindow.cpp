#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "m_macro.h"
#include "mg.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    mInit();
}

MainWindow::~MainWindow()
{
    m_tmr_ui.stop();
    delete ui;
}

void MainWindow::set_bk_color(QWidget *fml_wd, Qt::GlobalColor color)
{
    // 设置背景黑色
    // const对象不能调用非const函数，所以需要复制/设置并重新load
    QPalette pal(fml_wd->palette());
    pal.setColor(QPalette::Background, color);
    fml_wd->setAutoFillBackground(true);
    fml_wd->setPalette(pal);
    fml_wd->update();
}

void MainWindow::mInit()
{
    // 全局参数初始化
    mg_init();

    // 界面刷新定时器初始化
    m_tmr_ui.setInterval(100); //100ms刷新
    connect(&m_tmr_ui, &QTimer::timeout, this, &MainWindow::slt_tmr_ui_timeout);
    m_tmr_ui.start();
    // 添加设备显示
    for(short i=0;i<m_yh.m_devices.count();i++)
    {
        this->ui->cm_devices->addItem(m_yh.m_devices.at(i));
    }

    // 注意：需要在选择的硬件切换后更新
    m_yh.get_max_channels_ct();
    for(short i=0;i<m_yh.get_channels_used_count();i++)
    {
        this->ui->cm_chls->addItem(QString::number(i+1));
    }
    this->ui->cm_chls->setCurrentIndex(m_yh.get_channels_used_count()-1);

    // 传递采集指针
    m_sop.get_equ_pointer(&m_s7r, &m_yh, &m_fa);
}

void MainWindow::slt_tmr_ui_timeout()
{
    // 刷新界面显示时间
    mg_dTm = QDateTime::currentDateTime();
    this->ui->le_dtime->setText(mg_dTm.toString("yyyy-MM-dd hh:mm:ss"));
    this->ui->le_secs->setText(QString::number(mg_dTm.toSecsSinceEpoch()));
}

void MainWindow::on_cm_devices_currentIndexChanged(int index)
{
}

void MainWindow::on_pb_db_cnt_clicked()
{
    m_sop.sql_cnt();
    if(m_sop.isOpen())
    {
        set_bk_color(this->ui->frm_db, Qt::green);
    }
}

void MainWindow::on_pb_db_dCnt_clicked()
{
    m_sop.close();
    if(!m_sop.isOpen())
    {
        set_bk_color(this->ui->frm_db, Qt::lightGray);
    }
}

void MainWindow::on_pb_yh_st_clicked()
{
    ushort device_index = this->ui->cm_devices->currentIndex(); //获取当前选择的设备
    bool ok1, ok2, ok3;
    ushort use_chl_ct = ui->cm_chls->currentText().toInt(&ok1);
    uint rate = ui->le_rate->text().toInt(&ok2);
    uint up_rate = ui->le_up_per_sec->text().toInt(&ok3);
    if( ! (ok1&ok2&ok3))
    {
        QMessageBox::warning(this,"采集参数错误","设置的采集参数不是有效的数字");
        return;
    }

    m_yh.set_gat_para(use_chl_ct, rate, up_rate); //设置采集参数
    m_yh.set_hdr(device_index); //设置新的选择
    m_yh.start();

    // 设置背景颜色
    if(m_yh.isRunning())
    {
        set_bk_color(this->ui->frm_yh, Qt::green);
    }
}

void MainWindow::on_pb_yh_stp_clicked()
{
    m_yh.stop();
    // 设置背景颜色
    if(!m_yh.isRunning())
    {
        set_bk_color(this->ui->frm_yh, Qt::lightGray);
    }
}

void MainWindow::on_pb_fCnt_clicked()
{
    m_fa.cnnt();
    if(m_fa.isCnted())
    {
        set_bk_color(this->ui->frm_fanuc, Qt::green);
    }
}

void MainWindow::on_pb_fDisCnr_clicked()
{
    m_fa.close();
    set_bk_color(this->ui->frm_fanuc, Qt::lightGray);
}

void MainWindow::on_pb_fRd_info_clicked()
{
    m_fa.read_sys_info();
}

void MainWindow::on_pb_fRd_all_clicked()
{
    m_fa.read_all();
    this->ui->le_rd_tm->setText(QString::number(m_fa.rd_tm));
}

void MainWindow::on_pb_p_cnt_clicked()
{
    m_s7r.cnnt();
    if(m_s7r.isConnected())
    {
        set_bk_color(this->ui->frm_plc, Qt::green);
    }
}

void MainWindow::on_pb_p_disCnt_clicked()
{
    m_s7r.dis_cnnt();
    if(!m_s7r.isConnected())
    {
        set_bk_color(this->ui->frm_plc, Qt::lightGray);
    }
}
