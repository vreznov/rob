#-------------------------------------------------
#
# Project created by QtCreator 2018-07-23T10:27:15
#
#-------------------------------------------------

QT       += core gui network sql charts

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = nw_gat
TEMPLATE = app

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
        main.cpp \
        mainwindow.cpp \
    snap7/s7_client.cpp \
    snap7/s7_isotcp.cpp \
    snap7/s7_micro_client.cpp \
    snap7/s7_peer.cpp \
    snap7/snap_msgsock.cpp \
    snap7/snap_sysutils.cpp \
    snap7/snap_threads.cpp \
    mg.cpp \
    data_show.cpp \
    kfanuc.cpp \
    kfwriter.cpp \
    kchartview.cpp \
    kyh_ai16.cpp \
    kexception.cpp \
    ks7reader.cpp \
    kmatcacu.cpp \
    klogin.cpp

HEADERS += \
        mainwindow.h \
    m_macro.h \
    fw_header.h \
    bdaqctrl.h \
    snap7/s7_client.h \
    snap7/s7_isotcp.h \
    snap7/s7_micro_client.h \
    snap7/s7_peer.h \
    snap7/snap_msgsock.h \
    snap7/snap_sysutils.h \
    snap7/snap_threads.h \
    mg.h \
    m_enum.h \
    data_show.h \
    kfwriter.h \
    kchartview.h \
    kfanuc.h \
    kyh_ai16.h \
    kexception.h \
    ks7reader.h \
    mstc.h \
    Fwlib32.h \
    kmatcacu.h \
    klogin.h

FORMS += \
        mainwindow.ui \
    data_show.ui \
    klogin.ui

# ws2_32.lib WINDOWS SOCKET库
# winmm.lib windows标准库
LIBS += -lws2_32  -lwinmm
win32: LIBS += -L$$PWD/./ -lFwlib32
#win32: LIBS += -LD:\Matlab7\extern\lib\win32\microsoft\msvc70  -llibeng -llibmx -llibmex
win32: LIBS += -LD:\Matlab7\extern\lib\win32\microsoft\msvc70  -llibeng -llibmx -llibmex

#INCLUDEPATH += $$PWD/snap7  D:\Matlab7\extern\include
INCLUDEPATH += $$PWD/snap7 D:\Matlab7\extern\include
DISTFILES += \
    readme

RESOURCES += \
    rc1.qrc
