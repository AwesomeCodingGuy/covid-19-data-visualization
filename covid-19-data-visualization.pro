QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    src/data/appsettings.cpp \
    src/data/germany.cpp \
    src/main.cpp \
    src/models/germanydatatreemodel.cpp \
    src/models/treeitem.cpp \
    src/widgets/dashboard.cpp \
    src/widgets/germanycontentwidget.cpp \
    src/widgets/mainwindow.cpp

HEADERS += \
    src/data/appsettings.h \
    src/data/casedata.h \
    src/data/countrycode.h \
    src/data/germany.h \
    src/models/germanydatatreemodel.h \
    src/models/treeitem.h \
    src/widgets/dashboard.h \
    src/widgets/germanycontentwidget.h \
    src/widgets/mainwindow.h

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    data/data.qrc
