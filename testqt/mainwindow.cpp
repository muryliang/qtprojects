#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    qDebug() << "start of widget";
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::keyPressEvent(QKeyEvent *event) {
    qDebug() << "key pressed " << event->key();
    static int i = 1;
    if (event->key() == Qt::Key_1) {
        qDebug() << " pressed 1";
        QPushButton * btn = new QPushButton(this);
        btn->setText(QString::number(i++));
        ui->verticalLayout->addWidget(btn);
    }

}

