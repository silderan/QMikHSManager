#-------------------------------------------------
#
# Project created by QtCreator 2017-08-21T21:03:05
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = QMikHSManager
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    qconfigdata.cpp \
    DlgRouterConfig.cpp \
    MenuWrapper.cpp \
    Comm.cpp \
    QIniFile.cpp \
    QMD5.cpp \
    QSentences.cpp \
    QUserViewer.cpp \
    QROSInterface_ROSInfo.cpp \
    QROSInterface_Base.cpp \
    QROSInterface_HSUsers.cpp \
    QROSInterface_HSActive.cpp \
    Utils.cpp \
    QROSInterface_HSInfo.cpp \
    QROSInterface_Torch.cpp

HEADERS  += mainwindow.h \
    qconfigdata.h \
    DlgRouterConfig.h \
    MenuWrapper.h \
    Comm.h \
    QIniFile.h \
    QMD5.h \
    QSentences.h \
    QUserViewer.h \
    QROSInterface_ROSInfo.h \
    QROSInterface_Base.h \
    QROSInterface_HSUsers.h \
    QROSInterface_HSActive.h \
    Utils.h \
    QROSInterface_HSInfo.h \
    QROSInterface_Torch.h

FORMS    += mainwindow.ui \
    DlgRouterConfig.ui
