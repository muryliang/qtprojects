#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    port.setPortName("/dev/ttyS2");
    port.open(QIODevice::ReadOnly);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::keyPressEvent(QKeyEvent *event) {
    char buf[1024];
    int len = port.read(buf, sizeof(buf));
    qDebug() << "get len " << len;
    qDebug("get buffer :%s:\n", buf);
    qDebug() << "Done";
}
