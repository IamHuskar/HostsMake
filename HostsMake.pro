#-------------------------------------------------
#
# Project created by QtCreator 2016-05-11T10:24:53
#
#-------------------------------------------------

QT       += core gui xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets


TARGET = HostsMake
TEMPLATE = app


SOURCES += main.cpp\
        mainui.cpp \
    logger.cpp \
    lock.cpp \
    hostsxml.cpp \
    util.cpp \
    adddomaindialog.cpp \
    addnodedlg.cpp

HEADERS  += mainui.h \
    logger.h \
    lock.h \
    hostsxml.h \
    util.h \
    adddomaindialog.h \
    addnodedlg.h

FORMS    += mainui.ui \
    adddomaindialog.ui \
    addnodedlg.ui
