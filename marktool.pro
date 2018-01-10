#-------------------------------------------------
#
# Project created by QtCreator 2014-12-03T09:56:13
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = marktool
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    dataset.cpp \
    datascroll.cpp \
    dataedit.cpp \
    datarender.cpp \
    datarange.cpp

HEADERS  += mainwindow.h \
    dataset.h \
    datascroll.h \
    dataedit.h \
    datarender.h \
    datarange.h

FORMS    += mainwindow.ui
