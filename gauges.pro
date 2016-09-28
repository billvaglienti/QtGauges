#-------------------------------------------------
#
# Project created by QtCreator 2016-09-28T14:07:48
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = gauges
TEMPLATE = app

QMAKE_CXXFLAGS += -Wno-unused-parameter

SOURCES += main.cpp\
        dialog.cpp \
    attitudescene.cpp \
    dialscene.cpp \
    gauge.cpp \
    lineargaugescene.cpp \
    lineargaugescene2.cpp \
    twodimensionalplot.cpp \
    twodimensionalscene.cpp

HEADERS  += dialog.h \
    attitudescene.h \
    dialscene.h \
    gauge.h \
    lineargaugescene.h \
    lineargaugescene2.h \
    qgraphicsviewnoscroll.h \
    twodimensionalplot.h \
    twodimensionalscene.h

FORMS    += dialog.ui

DISTFILES += \
    LICENSE \
    README.md
