#-------------------------------------------------
#
# Project created by QtCreator 2017-10-24T19:25:54
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ILLW_client
TEMPLATE = app


SOURCES += main.cpp\
        clientwidget.cpp \
    signupwidget.cpp \
    findback.cpp \
    chatwidget.cpp \
    transwidget.cpp \
    mythread.cpp

HEADERS  += clientwidget.h \
    signupwidget.h \
    findback.h \
    chatwidget.h \
    transwidget.h \
    mythread.h

FORMS    += clientwidget.ui \
    signupwidget.ui \
    findback.ui \
    chatwidget.ui \
    transwidget.ui

CONFIG  += C++11
