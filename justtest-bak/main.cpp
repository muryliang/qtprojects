#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    int freq = 0;
    if (argc == 2) {
        freq = atoi(argv[1]);
    } else {
        freq = 0;
    }
    MainWindow w(freq);
    w.showFullScreen();
    w.show();

    return a.exec();
}
