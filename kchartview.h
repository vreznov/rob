#ifndef MCLS_CHARTVIEW_H
#define MCLS_CHARTVIEW_H
/********************************************
 * 文件名：chartview.h
 * 作者：Kare
 * 版本: 1.0
 * 功能：自定义的chart显示控件，继承标准控件。
 * 用于实现一些附加的数据处理功能
 ********************************************/
#include <QWidget>
#include <QChartView>

QT_CHARTS_USE_NAMESPACE

class KChartView : public QChartView
{
public:
    explicit KChartView(QWidget *parent = 0);
};

#endif // MCLS_CHARTVIEW_H
