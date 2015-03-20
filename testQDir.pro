#-------------------------------------------------
#
# Project created by QtCreator 2015-03-11T23:11:33
#
#-------------------------------------------------

QT       += core gui charts

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = testQDir
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    donutbreakdownchart.cpp \
    mainslice.cpp \

HEADERS  += mainwindow.h \
    donutbreakdownchart.h \
    mainslice.h \

FORMS    += mainwindow.ui
