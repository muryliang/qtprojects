#include "worker.h"
#include <QDebug>
#include <QThread>



Worker::Worker(int hz, int height, int ratio) :
    _hz(hz), _height(height), _ratio(ratio){

    frame= cv::imread("code.png");
    if (frame.channels() != 1) {
        cv::Mat planes[3];
        cv::split(frame, planes);
        mat = planes[1];
    } else {
        mat = frame;
    }
//    imgf = new QImage("code.png");
    imgf = new QImage(mat.data, mat.cols, mat.rows, QImage::Format_Indexed8);
}

Worker::~Worker() {
    delete imgf;
}

void Worker::process() {
//    int ratio = 2;
    QImage img = imgf->scaledToHeight(imgf->height() / _ratio);
    char *bits = reinterpret_cast<char*>(img.bits());
    int sleepTime = 1000 / _hz;


    int width = img.width();
    int height = img.height();
    int line = 0;
    while(true) {
        emit showImg(bits, line, width, _height, height);

        if (++line == height)
            line = 0;
//        qDebug() << "now line is " << line << height << width;
        QThread::msleep(sleepTime);
    }
}
