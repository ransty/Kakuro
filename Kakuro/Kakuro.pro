#-------------------------------------------------
#
# Project created by QtCreator 2016-10-13T10:40:18
#
# Software Development with C++ Assignment 2 - Kakuro
# Group Members:
# Oliver Norman - noroj001
# Shyam Pindoria - pinsy004
# Keano Porcaro - porkd002
#
#
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++14

TARGET = Kakuro
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
        usermove.cpp

HEADERS  += mainwindow.h \
         usermove.h

FORMS    += mainwindow.ui
