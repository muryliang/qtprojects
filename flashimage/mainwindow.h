#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "worker.h"
#include <QThread>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(int hz, int height = 80, int ratio = 4, QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    Worker *worker;
    QThread *thread;
    int _hz;
    int _height;
    int _ratio;

public slots:
    void recvData(char*, int, int, int, int);
};

#endif // MAINWINDOW_H
