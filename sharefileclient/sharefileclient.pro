#-------------------------------------------------
#
# Project created by QtCreator 2014-11-21T17:13:42
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = sharefileclient
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    clientcommand.cpp \
    signupdialog.cpp \
    connectdialog.cpp \
    ipwarning.cpp

HEADERS  += mainwindow.h \
    clientcommand.h \
    signupdialog.h \
    connectdialog.h \
    clientcommandmanager.h \
    ipwarning.h

FORMS    += mainwindow.ui \
    signupdialog.ui \
    connectdialog.ui \
    ipwarning.ui
