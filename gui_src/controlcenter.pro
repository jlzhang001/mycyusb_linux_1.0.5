TEMPLATE	= app
HEADERS		= ../include/controlcenter.h
FORMS		= controlcenter.ui
SOURCES		= controlcenter.cpp main.cpp fx2_download.cpp fx3_download.cpp streamer.cpp
LIBS		+= -L../lib -lcyusb -lusb-1.0
QT		+= network
QT += widgets
TARGET		= ../bin/cyusb_linux
CONFIG += c++11
CONFIG += c11
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x000000
