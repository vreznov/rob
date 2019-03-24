#include "data_show.h"
#include "ui_data_show.h"
#include "kyh_ai16.h"
#include "kfanuc.h"
#include "ks7reader.h"
#include <QValueAxis>

data_show::data_show(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::data_show)
{
    ui->setupUi(this);
    KInit();
}

data_show::~data_show()
{
    delete ui;
}

void data_show::KInit()
{
    InitUI();
}

void data_show::InitUI()
{
    //增加传感器选择
    QStringList sensor_list;
//    sensor_list<<"P1"<<"P2"<<"P3"<<"P4"<<"P5";
    sensor_list<<"主轴"<<"X轴"<<"Y轴电机"<<"Y轴螺母"<<"Z轴";
    ui->cb_sel->addItems(sensor_list);
    ui->cb_sel->setCurrentIndex(0);

    QStringList channel_list;
    channel_list<<"通道1"<<"通道2"<<"通道3"<<"通道4"<<"通道5"<<"通道6"<<"通道7"<<"通道8"<<"通道9"<<"通道10"<<"通道11"<<"通道12"<<"通道13"<<"通道14"<<"通道15";
    ui->cb_selCh->addItems(channel_list);

    //设置chart
    m_lineSeries[0].setName("X方向"); //设置曲线名称
    m_lineSeries[1].setName("Y方向");
    m_lineSeries[2].setName("Z方向");
    ui->m_chart_view->setChart(&m_chart);
    QValueAxis* axisX = new QValueAxis(&m_chart);
    QValueAxis* axisY = new QValueAxis(&m_chart);
    axisX->setRange(0,2000);
    axisY->setRange(-20, 20);
    m_chart.setAxisX(axisX);
    m_chart.setAxisY(axisY);

//    m_chart.createDefaultAxes();
    m_chart.setTitle("机床加速度传感器P1数据");

    //添加曲线到chart
    for(int i=0;i<3;i++)
    {
        m_chart.addSeries(&m_lineSeries[i]);
        m_lineSeries[i].attachAxis(axisX);
        m_lineSeries[i].attachAxis(axisY);
        m_lineSeries[i].setUseOpenGL(true); //使用OpenGL加速
    }

    ui->m_chart_view->setRenderHint(QPainter::Antialiasing); //消除锯齿
}

void data_show::getEquipPointer(KS7Reader *fml_s7, KYh_ai16 *fml_yh, KFanuc *fml_fa)
{
    m_s7r = fml_s7;
    m_yh = fml_yh;
    m_fanuc = fml_fa;

    // 连接信号槽
    connect(m_yh, &KYh_ai16::sig_data_ready, this, &data_show::slt_yh_trigged);
    connect(m_fanuc, &KFanuc::sig_mac_recvied, this, &data_show::slt_mac_received);
}

void data_show::slt_yh_trigged(uint fml_uiRcvCt)
{
    Q_UNUSED(fml_uiRcvCt)
    // 更新震动数据，只显示最后一个数据
    const PVEC2D_YH yh_data = m_yh->GetOriginalData();
    QLCDNumber* lcds[] = {
        ui->lcdNumber,
        ui->lcdNumber_2,
        ui->lcdNumber_3,
        ui->lcdNumber_4,
        ui->lcdNumber_5,
        ui->lcdNumber_6,
        ui->lcdNumber_7,
        ui->lcdNumber_8,
        ui->lcdNumber_9,
        ui->lcdNumber_10,
        ui->lcdNumber_11,
        ui->lcdNumber_12,
        ui->lcdNumber_13,
        ui->lcdNumber_14,
        ui->lcdNumber_15
    };
    for(int i=0;i<15;i++)
    {
        lcds[i]->display(yh_data->at(i).at(0).dat ); //j仅显示第一个数据
    }

//    QTime tm;
//    tm.start();
//    // 刷新chart
//    uint ct_per_ch = fml_uiRcvCt / m_yh->get_channels_used_count(); //每通道的数据量
//    const ushort use_ct = m_yh->get_channels_used_count();
//    m_ls2.clear();
//    for(int i=0;i<100;i++)
//    {
//        m_ls2.append(i, yh_data[i*use_ct]);
//    }
//    mc<<tm.elapsed()<<me;
    //更新chart显示，显示降采样后的数据，只显示选择的通道
    if(_currentSelectChannel >= MAX_CHANNEL)
    {
        mc<<"当前选择的图像显示索引无效"<<endl;
        return;
    }

    //添加数据到曲线显示
    QList<QPointF> pointsX, pointsY, pointsZ;
    const PVEC2D_YH pyh = m_yh->GetDownSamplingData();
    for(uint i=0;i<m_yh->GetCounterOfDownSampleChannel();i++)
    {
//        QPointF pnt(pyh->at(currentSelectChannel).at(i).timestamp, pyh->at(currentSelectChannel).at(i).dat);
        QPointF pntX(xindex, pyh->at(_currentSelectChannel * 3 + 0).at(i).dat);
        QPointF pntY(xindex, pyh->at(_currentSelectChannel * 3 + 1).at(i).dat);
        QPointF pntZ(xindex, pyh->at(_currentSelectChannel * 3 + 2).at(i).dat);
        pointsX.append(pntX);
        pointsY.append(pntY);
        pointsZ.append(pntZ);
        xindex++;
    }
    m_lineSeries[0].append(pointsX);
    m_lineSeries[1].append(pointsY);
    m_lineSeries[2].append(pointsZ);

    //如果数量超过N，则移动X轴及清除lineSeries顶部数据，确保不会无限占用内存
    if (m_lineSeries[0].count() > 2000)
    {
        m_lineSeries[0].removePoints(0, m_yh->GetCounterOfDownSampleChannel());
        //设置显示区间位 -2s~现在
//        m_chart.axisX()->setRange(pyh->at(currentSelectChannel).last().timestamp - 2000\
//                                  , pyh->at(currentSelectChannel).last().timestamp);
        m_chart.axisX()->setRange(xindex - 2000\
                                  , xindex);
    }
    else
    {
        m_chart.axisX()->setRange(0, 2000);
    }

    //计算方差
    m_mat->CacuMean(&m_yh->GetDownSamplingData()->at(0));
    double d[6];
    d[0] = m_mat->vMean;
    d[1] = m_mat->va;
    d[2] = m_mat->stDif;
    d[3] = m_mat->rm;
    d[4] = m_mat->ku;
    d[5] = m_mat->sk;
    QString meam = QString("均值：%8.4f, 方差:%8.4f, 标准差:%8.4f, 均方根:%8.4f, 峭度指标:%8.4f, 偏度指标:%8.4f")\
            .arg(d[0]) \
            .arg(d[1]) \
            .arg(d[2]) \
            .arg(d[3]) \
            .arg(d[4]) \
            .arg(d[5]);
    ui->plainTextEdit->setPlainText(meam);
}

void data_show::slt_mac_received()
{
    // 更新温度数据

    ui->lcdNumber_16->display(m_s7r->m_tempreture[0]);
    ui->lcdNumber_17->display(m_s7r->m_tempreture[1]);
    ui->lcdNumber_18->display(m_s7r->m_tempreture[2]);
    ui->lcdNumber_19->display(m_s7r->m_tempreture[3]);
    ui->lcdNumber_20->display(m_s7r->m_tempreture[4]);
    ui->lcdNumber_21->display(m_s7r->m_tempreture[5]);
    ui->lcdNumber_22->display(m_s7r->m_tempreture[6]);
    ui->lcdNumber_23->display(m_s7r->m_tempreture[7]);
    ui->lcdNumber_24->display(m_s7r->m_tempreture[8]);
    ui->lcdNumber_25->display(m_s7r->m_tempreture[9]);

    // 更新机床数据
    ui->lcdNumber_26->display(m_fanuc->m_spdl_speed[0]/1.0);
    ui->prgb_spld_load->setValue(m_fanuc->m_spdl_load[0]);

    ui->lcdNumber_27->display(m_fanuc->m_axis_feed_rate/1.0);

    ui->prgb_x_load->setValue(m_fanuc->m_axis_load[0]);
    ui->prgb_y_load->setValue(m_fanuc->m_axis_load[1]);
    ui->prgb_z_load->setValue(m_fanuc->m_axis_load[2]);

    ui->lcdNumber_32->display((double)m_fanuc->m_axis_speed[0]);
    ui->lcdNumber_33->display((double)m_fanuc->m_axis_speed[1]);
    ui->lcdNumber_34->display((double)m_fanuc->m_axis_speed[2]);
    ui->lcdNumber_35->display((double)m_fanuc->m_axis_crrnt[0]/1.0);
    ui->lcdNumber_36->display((double)m_fanuc->m_axis_crrnt[1]/1.0);
    ui->lcdNumber_37->display((double)m_fanuc->m_axis_crrnt[2]/1.0);
    ui->lcd_xAbs->display((double)m_fanuc->m_axis_abs_pos[0]);
    ui->lcd_yAbs->display((double)m_fanuc->m_axis_abs_pos[1]);
    ui->lcd_zAbs->display((double)m_fanuc->m_axis_abs_pos[2]);

    ui->label_60->setText(m_fanuc->m_macAuto ? "自动" : "手动");
    ui->label_58->setText(m_fanuc->m_macRun ? "运行" : "停止");
    ui->label_64->setText(m_fanuc->m_macAlarm ? "报警" : "正常");
    ui->label_66->setText(m_fanuc->m_macEmergency ? "紧急停止" : "正常");
    ui->label_55->setText(QString::number(m_fanuc->m_currentProgNo));
    ui->label_56->setText(QString::number(m_fanuc->m_mainPaogNo));


}

void data_show::on_cb_sel_currentIndexChanged(int index)
{
    QString qs = QString("机床加速度传感器位置P%1数据").arg(index + 1);
    _currentSelectChannel = index;
    xindex = 0; //reset x axis range
    m_lineSeries[0].clear();
    m_lineSeries[1].clear();
    m_lineSeries[2].clear();
    m_chart.setTitle(qs);
}

void data_show::on_cb_x_show_stateChanged(int arg1)
{
    if(arg1 != Qt::Checked)
    {
        m_lineSeries[0].hide();
    }
    else {
        m_lineSeries[0].show();
    }
}

void data_show::on_cb_y_show_stateChanged(int arg1)
{
    if(arg1 != Qt::Checked)
    {
        m_lineSeries[1].hide();
    }
    else {
        m_lineSeries[1].show();
    }
}

void data_show::on_cb_z_show_stateChanged(int arg1)
{
    if(arg1 != Qt::Checked)
    {
        m_lineSeries[2].hide();
    }
    else {
        m_lineSeries[2].show();
    }
}

void data_show::on_pushButton_clicked()
{
    int sel = ui->cb_selCh->currentIndex() + 1;
    m_mat->CacuInFile("E:/doc/MATLAB/vibra_2019_03_23_14_08_32.csv", sel);

    ui->le_mean->setText(QString::number((m_mat->vMean)));
    ui->le_var->setText(QString::number((m_mat->va)));
    ui->le_stDif->setText(QString::number((m_mat->stDif)));
    ui->le_rm->setText(QString::number((m_mat->rm)));
    ui->le_ku->setText(QString::number((m_mat->ku)));
    ui->le_sk->setText(QString::number((m_mat->sk)));
}
