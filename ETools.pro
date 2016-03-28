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
    qcustomplot.cpp \
    tempdist.cpp \
    lineform.cpp

HEADERS  += home.h \
    beam.h \
    beamview.h \
    qcustomplot.h \
    tempdist.h \
    lineform.h

FORMS    += home.ui \
    tempdist.ui \
    beam.ui \
    lineform.ui

RESOURCES += \
    resources.qrc

LIBS += \
       -lboost_system\
