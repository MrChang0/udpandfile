QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = UDPandFile
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    udpmutlticastsever.cpp \
    fileserver.cpp \
    tcpclient.cpp

HEADERS  += mainwindow.h \
    udpmutlticastsever.h \
    fileserver.h \
    tcpclient.h

FORMS    += mainwindow.ui
