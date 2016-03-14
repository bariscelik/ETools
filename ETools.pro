#-------------------------------------------------
#
# Project created by QtCreator 2016-03-06T07:16:20
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

TARGET = ETools
TEMPLATE = app

TRANSLATIONS += lang.ts
SOURCES += main.cpp\
    home.cpp \
    beam.cpp \
    beamview.cpp \
    qcustomplot.cpp

HEADERS  += home.h \
    beam.h \
    beamview.h \
    qcustomplot.h

FORMS    += home.ui \
    beam.ui

RESOURCES += \
    resources.qrc
