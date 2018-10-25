#include "mainwindow.h"
#include <QApplication>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    if (argc !=2 ) {
        qDebug() << "need gap number";
        return -1;
    }
    QApplication a(argc, argv);
    MainWindow w(atoi(argv[1]));
    w.show();

    return a.exec();
}
