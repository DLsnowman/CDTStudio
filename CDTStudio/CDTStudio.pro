#-------------------------------------------------
#
# Project created by QtCreator 2014-02-15T15:33:54
#
#-------------------------------------------------

QT       += core gui sql xml svg network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = CDTStudio
TEMPLATE = app
DESTDIR = ../bin

#PRECOMPILED HEADER
CONFIG+=precompile_header qwt
PRECOMPILED_HEADER = stable.h

SOURCES += main.cpp\
    mainwindow.cpp \
    cdtprojecttabwidget.cpp \
    cdtprojectwidget.cpp \
    cdtprojecttreeitem.cpp \
    cdtapplication.cpp \    
    cdtrecentfile.cpp

HEADERS  += \    
    mainwindow.h \
    cdtprojecttabwidget.h \
    cdtprojectwidget.h \
    cdtprojecttreeitem.h \
    cdtvariantconverter.h \
    cdtapplication.h \ 
    cdtrecentfile.h
	
FORMS    += \
    mainwindow.ui \

include(Dialogs/Dialogs.pri)
include(Docks/Docks.pri)
include(Helpers/Helpers.pri)
include(Layers/Layers.pri)
include(Wizards/Wizards.pri)
include(../Interfaces/Interfaces.pri)

INCLUDEPATH += \
    ../Interfaces \
    ../Tools\
    ../Tools/QPropertyEditor \
    ../Tools/CDTHistogramPlot \
    ../Tools/QtColorPicker\
    ../Tools/CDTFileSystem\
    ../Tools/CDTClassifierAssessmentWidget\
    ../Tools/CDTTableExporter\
    ../Tools/wwWidgets


DEPENDPATH += \
    ../Tools/QPropertyEditor \
    ../Tools/CDTHistogramPlot \
    ../Tools/QtColorPicker\
    ../Tools/CDTFileSystem\
    ../Tools/CDTClassifierAssessmentWidget\
    ../Tools/CDTTableExporter\
    ../Tools/wwWidgets

#Libraries
unix{
QMAKE_CXXFLAGS += -std=c++0x
LIBS += -lgdal -lgomp

INCLUDEPATH += /usr/include/gdal \
/usr/local/include/gdal \
/usr/include/qgis \
/usr/local/include/qgis

DEFINES += CORE_EXPORT=
DEFINES += GUI_EXPORT=
}
!unix{
include(../Tools/Config/win.pri)
LIBS += -lgdal_i -lgdi32
}

LIBS +=     -L../lib -lQPropertyEditor -lCDTHistogramPlot -lQtColorPicker\
             -lCDTFileSystem -lCDTTableExporter -lwwWidgets\
            -lqgis_core -lqgis_gui -lqgis_analysis -lqwt

win32:CONFIG(release, debug|release): LIBS += -lstxxl
else:win32:CONFIG(debug, debug|release): LIBS += -lstxxld
else:unix: LIBS += -lstxxl

#generate pdb for release version
#win32-msvc* {
#QMAKE_CFLAGS_RELEASE += -zi
#QMAKE_LFLAGS_RELEASE += /INCREMENTAL:NO /DEBUG
#}

#opencv
include(../Tools/Config/link2opencv.pri)

RESOURCES += \
    ../resource.qrc

DISTFILES +=
