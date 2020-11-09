#include "widgets/mainwindow.h"

#include <QApplication>
#include <QSettings>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QCoreApplication::setApplicationName("CovidDataVisualization");
    MainWindow w;
    w.show();
    return a.exec();
}
