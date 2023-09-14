#-------------------------------------------------
#
# Project created by QtCreator 2023-07-25T13:48:39
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = network2_07_IMClient
TEMPLATE = app



LIBS += -lWs2_32

SOURCES += main.cpp\
        dialog.cpp \
    mediator/UdpNetMediator.cpp \
    net/UdpNet.cpp \
    ckernel.cpp \
    mediator/INetMediator.cpp \
    useritem.cpp \
    chatdialog.cpp \
    net/TcpClient.cpp \
    net/TcpServer.cpp \
    mediator/TcpClientMediator.cpp \
    mediator/TcpServerMediator.cpp \
    logindialog.cpp

HEADERS  += dialog.h \
    mediator/INetMediator.h \
    mediator/UdpNetMediator.h \
    net/UdpNet.h \
    net/INet.h \
    net/packDef.h \
    ckernel.h \
    useritem.h \
    chatdialog.h \
    mediator/TcpClientMediator.h \
    mediator/TcpServerMediator.h \
    net/TcpClient.h \
    net/TcpServer.h \
    logindialog.h

FORMS    += dialog.ui \
    useritem.ui \
    chatdialog.ui \
    logindialog.ui

RESOURCES += \
    resource.qrc
