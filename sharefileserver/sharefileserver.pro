#-------------------------------------------------
#
# Project created by QtCreator 2014-11-21T17:27:12
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = sharefileserver
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    servercommand.cpp \
    task.cpp \
    user_db_editor.cpp

HEADERS  += mainwindow.h \
    servercommand.h \
    task.h\
    user_db_editor.h

FORMS    += mainwindow.ui
