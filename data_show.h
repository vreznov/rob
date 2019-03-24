#ifndef DATA_SHOW_H
#define DATA_SHOW_H

#include <QMainWindow>
#include <QChart>
#include <QChartView>
#include <QLineSeries>
#include "m_macro.h"
#include "kmatcacu.h"

class KYh_ai16;
class KFanuc;
class KS7Reader;

QT_CHARTS_USE_NAMESPACE

namespace Ui {
class data_show;
}

class data_show : public QMainWindow
{
    Q_OBJECT

public:
    explicit data_show(QWidget *parent = 0);
    ~data_show();

private:
    Ui::data_show *ui;
    void KInit();

    //! 初始化UI显示
    void InitUI();

    QChart m_chart; //chart对象

    //! 显示曲线，最多显示3条
    //! 太多了会增加计算负担，而且没有这个必要
    QLineSeries m_lineSeries[3];

    quint32 xindex = 0; //曲线图横坐标
    int _currentSelectChannel = 0; //当前选择的第N个位置的传感器
public:
    KS7Reader* m_s7r = nullptr;
    KYh_ai16* m_yh = nullptr;
    KFanuc* m_fanuc = nullptr;
    KMatCacu* m_mat = nullptr;

    //! 获取设备指针，用于在总界面传递设备指针，以便在类内对设备进行操作
    void getEquipPointer(KS7Reader* fml_s7, KYh_ai16* fml_yh, KFanuc* fml_fa);

public slots:
    //! 采集卡数据触发
    //! @param fml_uiRcvCt: 接收的数据数量
    void slt_yh_trigged(uint fml_uiRcvCt);

    //! 接收到了机床数据和PLC数据
    //! 同时在此更新机床数据和PLC数据
    void slt_mac_received();
private slots:
    //! 选择要显示曲线的传感器编号已改变
    void on_cb_sel_currentIndexChanged(int index);

    //! 隐藏X轴曲线
    void on_cb_x_show_stateChanged(int arg1);
    //! 隐藏Y轴曲线
    void on_cb_y_show_stateChanged(int arg1);
    //! 隐藏Z轴曲线
    void on_cb_z_show_stateChanged(int arg1);
    void on_pushButton_clicked();
};

#endif // DATA_SHOW_H
