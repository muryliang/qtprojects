﻿#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QFile>
#include <QMenu>
#include <QScrollBar>
#include <QImageWriter>


#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/videoio/videoio.hpp>


#include "Decoder.h"
#include "NDCode_Image_Recognition.h"

#ifdef __linux__
#include <stdio.h>
#endif


MainWindow::MainWindow(int gap, QWidget *parent) :
    QMainWindow(parent), _gap(gap),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->actionSave, SIGNAL(triggered(bool)), this, SLOT(saveImg()));


// one, overload qthread
//    runner = new mthread;
//    connect(runner, SIGNAL(sendImg(QImage)), this, SLOT(recvImg(QImage)), Qt::DirectConnection);
//    runner->start();

//    mImgData = new QByteArray(1024*1024, 0);

// two, move to thread
    qthread = new QThread();
    mworker = new Worker;
//    connect(mworker, SIGNAL(sendImg(QImage)), this, SLOT(recvImg(QImage))); // block is needed if origin image source is local variable
//    connect(mworker, SIGNAL(sendImgOne(QImage)), this, SLOT(recvImgOne(QImage)), Qt::QueuedConnection);
    connect(mworker, SIGNAL(sendImgData(QByteArray,int,int)), this, SLOT(recvImgData(QByteArray,int,int)), Qt::BlockingQueuedConnection);
    mworker->moveToThread(qthread);

    connect(qthread, SIGNAL(started()), mworker, SLOT(run()));
    qthread->start();

    // start timer when start loop of get from spi
    timer = new QTimer;
    connect(timer, SIGNAL(timeout()), this, SLOT(shout()));
    timer->setInterval(1000);
    timer->start(1000);


// third, show in mainthread
/*
    cv::Mat img2 = cv::imread("C:\\Users\\muryliang\\Pictures\\test images\\new2.png", -1);

    float scale = 0.25;
    cv::Size dsize = cv::Size(img2.cols*scale, img2.rows*scale);
    cv::Mat img = cv::Mat(dsize,CV_32S);
    cv::resize(img2, img, dsize);


       cv::imshow("123", img);    //“123”是显示框的名字
       cv::waitKey(100);     //通过整型变量a获取waitKey函数的返回值

//       destroyAllWindows();   //销毁窗口

       QVector<QByteArray> qb;
       for (int i = 0; i < 10; ++i) {
           cv::Rect rect(0, img.rows / 10 * i, img.cols, img.rows / 10);   //创建一个Rect框，属于cv中的类，四个参数代表x,y,width,height
           cv::Mat image_cut = cv::Mat(img, rect);      //从img中按照rect进行切割，此时修改image_cut时image中对应部分也会修改，因此需要copy
   //        cv::imshow("456",image_cut);
   //        cv::waitKey();
           if (image_cut.channels() == 4) {
               cv::Mat planes[4];
               cv::split(image_cut, planes);
               image_cut = planes[1];  // get green channel
           }

           qDebug() << "width and height and size " << image_cut.cols << image_cut.rows << image_cut.total() << image_cut.channels();
//           std::string strpath = "C:\\Users\\muryliang\\Pictures\\test images\\tmp\\cut";
           std::string strpath = "C:/Users/muryliang/Pictures/test images/tmp/cut";
           QFile f(QString::fromStdString(strpath) + QString::number(i) + ".raw");
           f.open(QIODevice::WriteOnly | QIODevice::Truncate);
           f.write((const char*)image_cut.data, image_cut.total());
           f.close();
           cv::imwrite(strpath + std::to_string(i) + ".bmp", image_cut);
           qb.append(QByteArray((const char*)image_cut.data, image_cut.total()));

           QImage tmpimg((const uchar*)image_cut.data, 640 * scale, 48 * scale, QImage::Format_Indexed8);

           QLabel *label = new QLabel;
           label->setPixmap(QPixmap::fromImage(tmpimg));
           ui->verticalLayout->addWidget(label);


       }

       QByteArray barr;
       for (int i = 0; i < 10; ++i) {
           barr.append(QByteArray((const char*)qb[i], 640 * scale * 48 * scale));
       }
       QImage mergedImg((const uchar*)barr.data(), 640 * scale, 480 * scale, QImage::Format_Indexed8);
       QLabel *label = new QLabel;
       label->setPixmap(QPixmap::fromImage(mergedImg));
       ui->verticalLayout->addWidget(label);
*/
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::recvImg(QImage img) {
    qDebug() << "begin show";
    QLabel *label = new QLabel;
    label->setPixmap(QPixmap::fromImage(img));
//    ui->horizontalLayout->addWidget(label);
    ui->verticalLayout->addWidget(label);

//    ui->label->setPixmap(QPixmap::fromImage(img));
}

void MainWindow::recvImgOne(QImage img) {
//    qDebug() << "begin show One";
    ui->label->setPixmap(QPixmap::fromImage(img));
//    ui->scrollArea->horizontalScrollBar()->setSliderPosition(ui->scrollArea->horizontalScrollBar()->maximum());
    ui->scrollArea->verticalScrollBar()->setSliderPosition(ui->scrollArea->verticalScrollBar()->maximum());
}

void MainWindow::recvImgData(QByteArray data, int width, int height)
{
//    if(mFlagImgDataUsed)
//        return;
//    mImgData = data;
//    mImgWidth = width;
//    mImgHeight++;
    int showHeight = 230;

    // make white even whiter
    int total = width * height;
    int gap = _gap;
    for (int i = 0; i < total; ++i) {
        if (data[i] > gap)
            data[i] = 0xff;
        else {
            data[i] = 0x0;
        }
    }

//    mImgData.append(data);
//    ui->label->setPixmap(QPixmap::fromImage(QImage((const uchar*)mImgData.data(), width, mImgHeight,QImage::Format_Indexed8)));
//qDebug() << "recvImgData  " << height;

    if (height > showHeight)
        ui->label->setPixmap(QPixmap::fromImage(QImage((const uchar*)data.data() + width * (height - showHeight), width, showHeight,QImage::Format_Indexed8)));
    else
        ui->label->setPixmap(QPixmap::fromImage(QImage((const uchar*)data.data(), width, height,QImage::Format_Indexed8)));

//    ui->scrollArea->verticalScrollBar()->setSliderPosition(ui->scrollArea->verticalScrollBar()->maximum());

    // set count
    static int picCount = 0;
    ui->label_2->setText(QString("count: ") + QString::number(++picCount));

    static int count = 0;
    test_decode(data.data(), width, height);
    QImageWriter writer("tmpimg" + QString::number(++count) + QString(".png"));
    writer.setFormat("png");
    writer.write(QImage((uchar*)data.data(), width, height, QImage::Format_Indexed8));


}

void MainWindow::shout()
{
//    mFlagImgDataUsed = true;

//    ui->label->setPixmap(QPixmap::fromImage(QImage((const uchar*)mImgData.data(), mImgWidth, mImgHeight, QImage::Format_Indexed8)));

//    ui->scrollArea->verticalScrollBar()->setSliderPosition(ui->scrollArea->verticalScrollBar()->maximum());

//    mFlagImgDataUsed = false;
    qDebug() << "1s";

}

void MainWindow::keyPressEvent(QKeyEvent* event) {
//    if (event->key() == Qt::Key_Up) {
//        emit change(1);
//    } else if (event->key() == Qt::Key_Down) {
//        emit change(0);
//    }
}

void MainWindow::saveImg() {
    QImageWriter writer("C:/Users/muryliang/Pictures/savedImg.png");
    writer.setFormat("png");
    writer.write(ui->label->pixmap()->toImage());
}

void MainWindow::test_decode(char* dataptr, int cols, int rows) {
    std::string ymlname = "/data/conf/algorithm_parameters_v01.yml";
    int bitarray[91], bound[4];
    uint8_t model_id = 0;
    char serial[9] = {0};

/*	for(int i = 0; i < 10; ++i)*/ {
        int ret = nanoid_hexagon_recognition((unsigned char*)dataptr, rows, cols, 1,
            const_cast<char*>(ymlname.c_str()), bitarray, 91,
            bound, 4);
        if (ret < 0) {
            printf("can not decode %d\n", ret);
            return;
        }

        printf("get byte code: \n");
        for (int k = 0; k < 91; ++k) {
            printf("num %d: %2x\n", k, bitarray[k]);
            if (k % 8 == 7) {
                putchar('\n');
            }
        }

        set_decoder_paras(1,5,0,1);
        ret = decoder(bitarray, 91, serial, 8, &model_id);
        if (ret < 0) {
            printf("can not get serial %d\n", ret);
        } else {
            printf("success get serial :%s:\n", serial);
            ui->label_3->setText(QString("serial :") + QString(serial));
        }
    }

    return;
}

