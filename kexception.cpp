#include "kexception.h"

KException::KException()
{
    code = 0;
}

KException::KException(const string &fml_sInfo, int fml_errId)
{
    msg = fml_sInfo;
    code = fml_errId;
}
