#ifndef KMATCACU_H
#define KMATCACU_H
/********************************************
 * 文件名：kmatcacu.h
 * 作者：Kare
 * 版本: 1.0
 * 功能：matlab引擎方式计算
 ********************************************/
#include <QObject>
#include "engine.h"
#include "m_macro.h"
#include "mstc.h"

class KMatCacu : public QObject
{
    Q_OBJECT
public:
    explicit KMatCacu(QObject *parent = nullptr);

    bool InitMatlabEngine();
    bool StopMatlabEngine();
    void TestSinx();
    Engine* ep;

    //! 引用matlab引擎计算
    //! @param fml_d:降采样后的数据
    void CacuInMatlab(const PVEC2D_YH fml_d);
    void CacuInMatlab(const double* fml_d);

    //! 计算指定文件中的数据
    void CacuInFile(QString fml_sFn, int fml_n);

    //! 计算特征值
    //! @param fml_d: 一个通道的
    void CacuMean(const PVEC_YH fml_d);

    void GetResult();

    double vMean, va, stDif, rm, ku, sk;
    mxArray* pmxvMean = nullptr;
    mxArray* pmxva = nullptr;
    mxArray* pmxstDif = nullptr;
    mxArray* mxprm = nullptr;
    mxArray* pmxku = nullptr;
    mxArray* pmxsk = nullptr;
signals:

public slots:
};

#endif // KMATCACU_H
