#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QImage>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/videoio/videoio.hpp>
#include <QtGlobal>


MainWindow::MainWindow(int hz, int height, int ratio, QWidget *parent) :
    _hz(hz), _height(height), _ratio(ratio), QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    worker = new Worker(_hz, _height, _ratio);
    connect(worker, SIGNAL(showImg(char* , int, int , int, int)), this, SLOT(recvData(char* , int, int, int, int)));

    thread = new QThread();
    worker->moveToThread(thread);

    connect(thread, SIGNAL(started()), worker, SLOT(process()));
    thread->start();

}

MainWindow::~MainWindow()
{
    delete ui;
    thread->deleteLater();
    worker->deleteLater();
}

void MainWindow::recvData(char* data, int line, int width, int height, int totalHeight) {
    QImage img((uchar*)(data + line * width), width, qMin(height, totalHeight - line), QImage::Format_Indexed8);
    ui->label->setPixmap(QPixmap::fromImage(img));
}
