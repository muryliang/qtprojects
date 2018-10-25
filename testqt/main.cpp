#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    qputenv("QT_QPA_EVDEV_KEYBOARD_PARAMETERS", "/dev/event100");
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
