QT       += core gui charts network concurrent

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

TARGET = CovidDataVisualization

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    src/data/appsettings.cpp \
    src/data/casedata.cpp \
    src/data/constants.cpp \
    src/data/coviddata.cpp \
    src/data/coviddatatreeitem.cpp \
    src/data/reader/germanydatareader.cpp \
    src/data/reader/usadatareader.cpp \
    src/data/reader/worlddatareader.cpp \
    src/main.cpp \
    src/models/coviddatatreemodel.cpp \
    src/models/treeitem.cpp \
    src/utils/areaitem.cpp \
    src/utils/downloadmanager.cpp \
    src/utils/shapelibutils.cpp \
    src/utils/utility.cpp \
    src/widgets/chartwidget.cpp \
    src/widgets/maincontentwidget.cpp \
    src/widgets/mainwindow.cpp \
    src/widgets/mapview.cpp \
    src/widgets/mapwidget.cpp \
    src/widgets/treeview.cpp

HEADERS += \
    src/data/appsettings.h \
    src/data/casedata.h \
    src/data/constants.h \
    src/data/coviddata.h \
    src/data/coviddatatreeitem.h \
    src/data/mapregion.h \
    src/data/reader/germanydatareader.h \
    src/data/reader/usadatareader.h \
    src/data/reader/worlddatareader.h \
    src/models/coviddatatreemodel.h \
    src/models/treeitem.h \
    src/utils/areaitem.h \
    src/utils/downloadmanager.h \
    src/utils/graphicsitems.h \
    src/utils/shapelibutils.h \
    src/utils/utility.h \
    src/widgets/chartwidget.h \
    src/widgets/maincontentwidget.h \
    src/widgets/mainwindow.h \
    src/widgets/mapview.h \
    src/widgets/mapwidget.h \
    src/widgets/treeview.h

# shapelib
INCLUDEPATH += $$PWD/sub/shapelib
SOURCES += \
    sub/shapelib/shpopen.c \
    sub/shapelib/dbfopen.c \
    sub/shapelib/safileio.c

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    data/data.qrc
