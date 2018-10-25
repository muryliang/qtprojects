#ifndef WORKER_H
#define WORKER_H

#include <QObject>
#include <QImage>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/videoio/videoio.hpp>

class Worker : public QObject {
    Q_OBJECT
public:
    Worker(int hz, int height = 80, int ratio = 4);

    ~Worker();

public slots:
    void process();

private:
    QImage* imgf;
    cv::Mat mat, frame;
    int _hz;
    int _height;
    int _ratio;

signals:
    void showImg(char* , int, int , int, int);
};
#endif // WORKER_H
