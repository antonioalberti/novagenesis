#-------------------------------------------------
#
# Project created by QtCreator 2014-09-28T12:09:51
#
#-------------------------------------------------

QT       += core gui webkitwidgets dbus

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = NGBrowser
TEMPLATE = app

DBUS_ADAPTORS += ngconnectorif.xml
DBUS_INTERFACES += ngcommunication.xml

SOURCES += main.cpp\
        mainwindow.cpp \
    ngurl.cpp \
    ngconnector.cpp \
    ngtemplate.cpp

HEADERS  += mainwindow.h \
    ngurl.h \
    ngconnector.h \
    NGConnectorIf.h \
    global.h \
    ngtemplate.h

FORMS    += mainwindow.ui

DISTFILES += \
    ngs-mainwindow-tmpl.html \
    ngs-item-mainwindow-tmpl.html

