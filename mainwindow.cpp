#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "m_macro.h"
#include "mg.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    KInit();
}

MainWindow::~MainWindow()
{
    m_tmr_ui.stop();
    m_wndDatShow.close();
    delete ui;
}

void MainWindow::SetBackcolor(QWidget *fml_wd, Qt::GlobalColor color)
{
    // 设置背景黑色
    // const对象不能调用非const函数，所以需要复制/设置并重新load
    QPalette pal(fml_wd->palette());
    pal.setColor(QPalette::Background, color);
    fml_wd->setAutoFillBackground(true);
    fml_wd->setPalette(pal);
    fml_wd->update();
}

void MainWindow::KInit()
{
    // 全局参数初始化
    mg_init();
    _tmGather.start();

    // 界面刷新定时器初始化
    m_tmr_ui.setInterval(200); //xxxms刷新
    connect(&m_tmr_ui, &QTimer::timeout, this, &MainWindow::slt_tmr_ui_timeout);
    // 添加设备显示
    for(short i=0;i<m_yh.m_devices.count();i++)
    {
        this->ui->cm_devices->addItem(m_yh.m_devices.at(i));
    }

    // 注意是否有BUG：需要在选择的硬件切换后更新
    // 添加通道选择
    m_yh.ReqMaxChannel();
    for(short i=0;i<MAX_CHANNEL;i++)
    {
        this->ui->cm_chls->addItem(QString::number(i+1));
    }
    this->ui->cm_chls->setCurrentIndex(m_yh.GetUsedChannel()-1);

    // 添加倍率选择
    this->ui->cm_amp->addItems(QStringList()<<"1"<<"2"<<"5"<<"10");

    // 传递采集指针
    m_fWriter.get_equ_pointer(&m_s7r, &m_yh, &m_fanuc);
    m_wndDatShow.getEquipPointer(&m_s7r, &m_yh, &m_fanuc);
    m_wndDatShow.m_mat = &_cacu;

    //信号槽连接
    connect(&m_fanuc, &KFanuc::sig_mac_run, this, &MainWindow::slt_mac_run);
    connect(&m_fanuc, &KFanuc::sig_mac_stop, this, &MainWindow::slt_mac_stop);
}

void MainWindow::slt_tmr_ui_timeout()
{
    // 刷新界面显示时间
    mg_dTm = QDateTime::currentDateTime();
    this->ui->le_dtime->setText(mg_dTm.toString("yyyy-MM-dd hh:mm:ss"));
    this->ui->le_secs->setText(QString::number(_tmGather.elapsed() / 1000));

    m_fWriter.m_recored = m_bRec;
    // 查询机床数据和PLC数据
//    m_fanuc.Req();
    KFanucThread pFanuc(&m_fanuc);
    ui->le_rd_tm->setText(QString::number(m_fanuc.m_readSpendTime));
    m_s7r.Req();
    if(!m_yh.IsRunning())
    {
        m_yh.Start();
    }
}

void MainWindow::on_pb_yh_st_clicked()
{
    ushort device_index = this->ui->cm_devices->currentIndex(); //获取当前选择的设备
    bool ok1, ok2, ok3;
    ushort use_chl_ct = ui->cm_chls->currentText().toInt(&ok1);
    uint rate = ui->le_rate->text().toInt(&ok2);
    uint upload_rate = ui->le_up_per_sec->text().toInt(&ok3);
    if( ! (ok1&ok2&ok3))
    {
        QMessageBox::warning(this,"采集参数错误","设置的采集参数不是有效的数字");
        return;
    }

    m_yh.SetGatherParameter(use_chl_ct, rate, upload_rate); //设置采集参数
    int ec = m_yh.SendParam2Hardware(device_index); //设置新的选择
    if(ec == 0)
    {
        SetBackcolor(this->ui->frm_yh, Qt::green);
    }
    else
    {
        SetBackcolor(this->ui->frm_yh, Qt::gray);
    }
//    m_yh.Start();

//    // 设置背景颜色
//    if(m_yh.IsRunning())
//    {
//        set_bk_color(this->ui->frm_yh, Qt::green);
//    }
}

void MainWindow::on_pb_yh_stp_clicked()
{
    m_yh.Stop();
    // 设置背景颜色
    if(!m_yh.IsRunning())
    {
        SetBackcolor(this->ui->frm_yh, Qt::lightGray);
    }
}

void MainWindow::on_pb_fCnt_clicked()
{
    m_fanuc.Connect2Device();
    if(m_fanuc.isContected())
    {
        SetBackcolor(this->ui->frm_fanuc, Qt::green);
    }
}

void MainWindow::on_pb_fDisCnr_clicked()
{
    m_fanuc.close();
    SetBackcolor(this->ui->frm_fanuc, Qt::lightGray);
}

void MainWindow::on_pb_fRd_info_clicked()
{
    m_fanuc.ReadMachineSystemInfo();
}

void MainWindow::on_pb_fRd_all_clicked()
{
    m_fanuc.ReadAllInfo();
    this->ui->le_rd_tm->setText(QString::number(m_fanuc.m_readSpendTime));
}

void MainWindow::on_pb_p_cnt_clicked()
{
    m_s7r.cnnt();
    if(m_s7r.isConnected())
    {
        SetBackcolor(this->ui->frm_plc, Qt::green);
    }
}

void MainWindow::on_pb_p_disCnt_clicked()
{
    m_s7r.dis_cnnt();
    if(!m_s7r.isConnected())
    {
        SetBackcolor(this->ui->frm_plc, Qt::lightGray);
    }
}

void MainWindow::on_pb_gat_start_clicked()
{
    _tmGather.restart();
    // 打开新的窗口
    m_wndDatShow.show();
    m_tmr_ui.start();
}

void MainWindow::on_pb_gat_stop_clicked()
{
    m_yh.Stop();
    m_tmr_ui.stop();
    ui->checkBox->setChecked(false); //停止文件记录
}

void MainWindow::on_cm_amp_currentIndexChanged(const QString &arg1)
{
    bool ok;
    ushort fac = arg1.toUShort(&ok);
    if(ok)
    {
        m_yh.SetAmplificationFactor(fac);
    }
}

void MainWindow::on_pb_tst_mac_clicked()
{
    m_fWriter.slt_mac_recvied();
}

void MainWindow::on_checkBox_stateChanged(int arg1)
{
    m_bRec = arg1==Qt::Checked;
    if(m_bRec)
    {
        m_fWriter.CreatFile();
        SetBackcolor(ui->frame, Qt::green);
    }
    else
    {
        m_fWriter.CloseFile();
        SetBackcolor(ui->frame, Qt::white);
    }
}

void MainWindow::slt_mac_run()
{
    ui->checkBox->setChecked(true); //触发文件记录
}

void MainWindow::slt_mac_stop()
{
    ui->checkBox->setChecked(false); //停止文件记录
}

void MainWindow::on_pb_tst_crt_file_clicked()
{
    m_fWriter.CreatFile();
    m_fWriter.m_recored = true;
}

void MainWindow::on_pb_tst_close_file_clicked()
{
    m_bRec = false;
    m_fWriter.CloseFile();
    m_fWriter.m_recored = false;
}

void MainWindow::on_pb_rd_progline_clicked()
{
    m_fanuc.ReadCurrentProgLine();
}

void MainWindow::on_pushButton_clicked()
{
    _cacu.InitMatlabEngine();
}

void MainWindow::on_pushButton_2_clicked()
{
    _cacu.StopMatlabEngine();
}

void MainWindow::on_pushButton_3_clicked()
{
    _cacu.TestSinx();
}

void MainWindow::on_pushButton_4_clicked()
{
    _cacu.CacuInFile("E:/doc/MATLAB/vibra_2019_03_23_14_08_32.csv", 2);
}
