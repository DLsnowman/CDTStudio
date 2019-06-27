QT       += core

TARGET = SVM
TEMPLATE = lib
CONFIG += plugin
DESTDIR = ../../../bin/Plugins

SOURCES += svminterface.cpp

HEADERS += svminterface.h

include(../../../Interfaces/Interfaces.pri)

OTHER_FILES += SVM.json

unix{
QMAKE_CXXFLAGS += -std=c++0x
target.path = /usr/lib
INSTALLS += target
}
!unix{
include(../../../Tools/Config/win.pri)
}

#opencv
include(../../../Tools/Config/link2opencv.pri)
