#include "worker.h"
#include <QCoreApplication>
#include <QDebug>
#include <QFile>
#include <QTime>
#include <QTimer>
#include <QThread>
#include <QMatrix>
#include <QSize>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

//#include <windows.h>
//#include <M3F20xm.h>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/videoio/videoio.hpp>


// while get fixed length of data from spi
// append data into image data buf, add count value,
// if count % interval == 0
//      do a recognition

Worker::Worker(int freq) {
    if (freq <= 0) {
        freq = 10;
    }
    sleep_time = 1000 / freq;
}

#if 0
void Worker::run() {
    cv::Mat img2 = cv::imread("C:/Users/muryliang/Pictures/test images/new2a.png", -1);

    float scale = 1;
    int slot = 480;
    int height = 480, width = 640;
    cv::Size dsize = cv::Size(img2.cols*scale, img2.rows*scale);
    cv::Mat img = cv::Mat(dsize,CV_32S);
    cv::resize(img2, img, dsize);

//    cv::imshow("123", img);    //“123”是显示框的名字
//    cv::waitKey(100);     //通过整型变量a获取waitKey函数的返回值


//       cv::imshow("123", img);    //“123”是显示框的名字
//       cv::waitKey(100);     //通过整型变量a获取waitKey函数的返回值
        QMatrix matrix;
//        matrix.translate(width * scale / 2, height * scale / 2);
        matrix.rotate(270);

       for (int i = 0; i < slot; ++i) {
           cv::Rect rect(0, img.rows / slot * i, img.cols, img.rows / slot);   //创建一个Rect框，属于cv中的类，四个参数代表x,y,width,height
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
//           std::string strpath = "C:/Users/muryliang/Pictures/test images/tmp/cut";
//           QFile f(QString::fromStdString(strpath) + QString::number(i) + ".raw");
//           f.open(QIODevice::WriteOnly | QIODevice::Truncate);
//           f.write((const char*)image_cut.data, image_cut.total());
//           f.close();
//           cv::imwrite(strpath + std::to_string(i) + ".bmp", image_cut);
           qb.append(QByteArray((const char*)image_cut.data, image_cut.total()));

//           QImage tmpimg((const uchar*)image_cut.data, width * scale, height * scale / slot, QImage::Format_Indexed8);
//           emit sendImg(tmpimg.transformed(matrix));
       }


       for (int i = 0; i < slot; ++i) {
           barr.append(QByteArray((const char*)qb[i], width * scale * height * scale / slot));
       }
       QImage mergedImg((const uchar*)barr.data(), width * scale, height * scale, QImage::Format_Indexed8);

       emit sendImg(mergedImg);
//       QCoreApplication::processEvents();


}
#else

#define WIDTH 300


void Worker::run() {
    barr.reserve(1500*1000*10);
    int count = 1;
    char buf[WIDTH + 3];

    QMatrix matrix;
    matrix.rotate(270);

//    emit startloop();
    // read loop
    QVector<QByteArray> qb;

    int fd;
    if ((fd = open("/dev/cam", O_RDONLY)) == -1) {
        qDebug() << "can not open camere device";
        return;
    }

    int wtimes = 3;  // width timers expand
    while (true) {
        memset(buf, 0, WIDTH+3); // clear buffer head

        if (read(fd, buf, sizeof(buf)) == sizeof(buf)) {
            if (buf[0] == (char)0x00 && buf[1] == (char)0xff && buf[2] == (char)0x01) {
                cv::Mat tmpmat1(1, WIDTH/2, CV_8UC1, buf+3);
                cv::Mat tmpmat2;
                cv::resize(tmpmat1, tmpmat2, cv::Size(WIDTH, wtimes)); // expand
//                qDebug() << "total is " << tmpmat1.total() <<  tmpmat2.total();
                barr.append(QByteArray((const char*)(tmpmat2.data), tmpmat2.total()));
//                printf("%d %d %d %d\n", tmpmat2.total(), barr.size(), WIDTH/2, count*wtimes);
                emit sendImgOne(QImage((const uchar*)barr.data(), WIDTH, count*wtimes, QImage::Format_Indexed8).transformed(matrix));

                ++count;
            }


        } else {
            qDebug() << "read size not right";
        }


        QThread::msleep(sleep_time);
    }
    close(fd);

    qDebug() << "done scan";
}

#endif

