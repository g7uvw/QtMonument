#-------------------------------------------------
#
# Project created by QtCreator 2015-08-05T11:33:51
#
#-------------------------------------------------

QT       += core gui serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Monument
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    settingsdialog.cpp \
    motor.cpp

HEADERS  += mainwindow.h \
    settingsdialog.h \
    motor.h \
    serial.h

FORMS    += mainwindow.ui \
    settingsdialog.ui
