#include "kmatcacu.h"
#include <math.h>

KMatCacu::KMatCacu(QObject *parent) : QObject(parent)
{

}

bool KMatCacu::InitMatlabEngine()
{
    ep = engOpen(NULL);
    if (ep  == NULL ) {
        mc << "Can't start MATLAB engine!";
        return false;
    }
    engEvalString(ep, "cd('E:/doc/MATLAB/'");
    return true;
}

bool KMatCacu::StopMatlabEngine()
{
    int ret = engClose(ep);
    return false;
}

void KMatCacu::TestSinx()
{
    engEvalString(ep, "ezplot('sin(x)')");
}

void KMatCacu::CacuInMatlab(const QVector<QVector<DataPoint> > *fml_d)
{
//    const int length = fml_d->at(0).length();
//    mxArray *marray = mxCreateDoubleMatrix(length, MAX_CHANNEL + 1, mxREAL);
//    double* pd = mxGetPr(marray); //获取指针
//    for(int i=0;i<length;i++) //将数据写入matlab对象
//    {
//        pd[i*MAX_CHANNEL] = fml_d->at(0).at(i).dtime.toMSecsSinceEpoch();
//        for(int j=1;j<MAX_CHANNEL+1;j++)
//        {
//            pd[i*MAX_CHANNEL+j] = fml_d->at(j).at(i).dat;
//        }
//    }
//    engEvalString(ep, "cd C:\\Users\\wht_t\\Desktop\\test\\Project1\\x64\\Release");
}

void KMatCacu::CacuInMatlab(const double *fml_d)
{

}

void KMatCacu::CacuInFile(QString fml_sFn, int fml_n)
{
    QString cmd = "row=" + QString::number(fml_n) + ";" \
            + "filename='" + fml_sFn + "';spectrum_file";
//    int dim = 1;
//    mxArray* pRow = mxCreateNumericArray(1, &dim, mxINT32_CLASS, mxREAL);
//    memcpy((char *) mxGetPr(pRow), (char *) (&fml_n), 1*sizeof(double));
//    engPutVariable(ep, "row", pRow);

    int ret = engEvalString(ep, cmd.toStdString().c_str());
    GetResult();

}

void KMatCacu::CacuMean(const QVector<DataPoint> *fml_d)
{
    if(fml_d->length() == 0) return;
    mxArray* py = mxCreateDoubleMatrix(1, fml_d->length(), mxREAL);
    double* dat = new double[fml_d->length()];
    for(int i=0;i<fml_d->length();i++)
    {
        dat[i] = fml_d->at(i).dat;
    }
    memcpy((char *) mxGetPr(py), (char *) (dat), fml_d->length());
    engPutVariable(ep, "y", py);
    engEvalString(ep, "T_domain1");
    GetResult();
    delete[] dat;
}

void KMatCacu::GetResult()
{

    //获取计算结果 获取matlab中变量的值
    pmxvMean = engGetVariable(ep, "me");
    pmxva = engGetVariable(ep, "va");
    pmxstDif = engGetVariable(ep, "st");
    mxprm = engGetVariable(ep, "rm");
    pmxku = engGetVariable(ep, "ku");
    pmxsk = engGetVariable(ep, "sk");

    vMean = *mxGetPr(pmxvMean);
    va = *mxGetPr(pmxva);
    stDif = *mxGetPr(pmxstDif);
    rm = *mxGetPr(mxprm);
    ku = *mxGetPr(pmxku);
    sk = *mxGetPr(pmxsk);
}

