#ifndef MS7_GLOBAL_H
#define MS7_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(MS7_LIBRARY)
#  define MS7SHARED_EXPORT Q_DECL_EXPORT
#else
#  define MS7SHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // MS7_GLOBAL_H
