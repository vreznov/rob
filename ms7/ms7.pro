#-------------------------------------------------
#
# Project created by QtCreator 2018-07-25T10:43:07
#
#-------------------------------------------------

QT       -= gui
QT += network
TARGET = ms7
TEMPLATE = lib

DEFINES += MS7_LIBRARY

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    s7_client.cpp \
    s7_isotcp.cpp \
    s7_micro_client.cpp \
    s7_partner.cpp \
    s7_peer.cpp \
    s7_server.cpp \
    s7_text.cpp \
    snap_msgsock.cpp \
    snap_sysutils.cpp \
    snap_tcpsrvr.cpp \
    snap_threads.cpp \
    snap7_libmain.cpp

HEADERS += \
        ms7_global.h \ 
    s7_client.h \
    s7_firmware.h \
    s7_isotcp.h \
    s7_micro_client.h \
    s7_partner.h \
    s7_peer.h \
    s7_server.h \
    s7_text.h \
    s7_types.h \
    snap_msgsock.h \
    snap_platform.h \
    snap_sysutils.h \
    snap_tcpsrvr.h \
    snap_threads.h \
    sol_threads.h \
    win_threads.h \
    snap7_libmain.h

LIBS +=  -lws2_32 -lwinmm

unix {
    target.path = /usr/lib
    INSTALLS += target
}
