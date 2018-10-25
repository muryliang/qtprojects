#include "mainwindow.h"
#include <QApplication>
#include <stdlib.h>
#include <QDebug>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    if (argc != 4) {
        qDebug() << "need hz and height and ratio";
        return -1;
    }
    MainWindow w(atoi(argv[1]), atoi(argv[2]), atoi(argv[3]));
    w.setStyleSheet("QMainWindow {background: 'black';}");
    w.show();

    return a.exec();
}
