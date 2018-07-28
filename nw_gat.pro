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
    fanuc.cpp \
    yh_ai16.cpp \
    s7_reader.cpp \
    mh/s7_client.cpp \
    mh/s7_isotcp.cpp \
    mh/s7_micro_client.cpp \
    mh/s7_peer.cpp \
    mh/snap_msgsock.cpp \
    mh/snap_sysutils.cpp \
    mh/snap_threads.cpp \
    mysql_opera.cpp \
    mg.cpp

HEADERS += \
        mainwindow.h \
    fanuc.h \
    m_macro.h \
    fw_header.h \
    yh_ai16.h \
    bdaqctrl.h \
    s7_reader.h \
    mh/s7_client.h \
    mh/s7_isotcp.h \
    mh/s7_micro_client.h \
    mh/s7_peer.h \
    mh/snap_msgsock.h \
    mh/snap_sysutils.h \
    mh/snap_threads.h \
    mysql_opera.h \
    mg.h

FORMS += \
        mainwindow.ui

LIBS += -lws2_32  -lwinmm

INCLUDEPATH += E:\kare\doc\QtProg\nw_gat\mh
