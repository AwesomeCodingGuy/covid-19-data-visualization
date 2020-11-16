QT       += core gui charts network

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
    src/data/germany.cpp \
    src/data/usa.cpp \
    src/data/world.cpp \
    src/main.cpp \
    src/models/germanydatatreemodel.cpp \
    src/models/treeitem.cpp \
    src/models/usadatatreemodel.cpp \
    src/models/usatreeitem.cpp \
    src/models/worlddatatreemodel.cpp \
    src/models/worldtreeitem.cpp \
    src/utils/areaitem.cpp \
    src/utils/downloadmanager.cpp \
    src/utils/shapelibutils.cpp \
    src/utils/utility.cpp \
    src/widgets/chartwidget.cpp \
    src/widgets/dashboard.cpp \
    src/widgets/germanycontentwidget.cpp \
    src/widgets/germanymapwidget.cpp \
    src/widgets/mainwindow.cpp \
    src/widgets/mapview.cpp \
    src/widgets/usacontentwidget.cpp \
    src/widgets/worldcontentwidget.cpp

HEADERS += \
    src/data/appsettings.h \
    src/data/casedata.h \
    src/data/constants.h \
    src/data/countrycode.h \
    src/data/germany.h \
    src/data/usa.h \
    src/data/world.h \
    src/models/germanydatatreemodel.h \
    src/models/treeitem.h \
    src/models/usadatatreemodel.h \
    src/models/usatreeitem.h \
    src/models/worlddatatreemodel.h \
    src/models/worldtreeitem.h \
    src/utils/areaitem.h \
    src/utils/downloadmanager.h \
    src/utils/graphicsitems.h \
    src/utils/shapelibutils.h \
    src/utils/utility.h \
    src/widgets/chartwidget.h \
    src/widgets/dashboard.h \
    src/widgets/germanycontentwidget.h \
    src/widgets/germanymapwidget.h \
    src/widgets/mainwindow.h \
    src/widgets/mapview.h \
    src/widgets/usacontentwidget.h \
    src/widgets/worldcontentwidget.h

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
