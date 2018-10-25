#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "pkghandle.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QString confpath, QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    PkgHandle *handle;

private slots:
    void handleinstall();
    void handleuninstall();
};

#endif // MAINWINDOW_H
