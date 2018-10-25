#ifndef WORKER_H
#define WORKER_H

#include <QQueue>
#include <QImage>
#include <QByteArray>


class Worker : public QObject {
    Q_OBJECT

public:
    Worker(int freq);

public slots:
    void run();

signals:
    void sendImg(QImage);
    void sendImgOne(QImage);
    void startloop();


private:
    QVector<QByteArray>  qb;
    QByteArray barr;
    int sleep_time;
};

#endif // WORKER_H
