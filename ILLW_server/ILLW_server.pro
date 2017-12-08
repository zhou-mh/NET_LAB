#-------------------------------------------------
#
# Project created by QtCreator 2017-10-24T19:26:34
#
#-------------------------------------------------

QT       += core gui network sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ILLW_server
TEMPLATE = app


SOURCES += main.cpp\
        serverwidget.cpp \
    sqlite.cpp

HEADERS  += serverwidget.h \
    sqlite.h

FORMS    += serverwidget.ui \
    sqlite.ui

CONFIG  += C++11
