#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QImage>
#include <mthread.h>
#include <worker.h>
#include <QTimer>
#include <QKeyEvent>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(int freq, QWidget *parent = 0);
    ~MainWindow();
protected:
    void keyPressEvent(QKeyEvent* event) override;


private:
    Ui::MainWindow *ui;
    mthread *runner;
    QThread *qthread;
    Worker *mworker;
    QTimer *timer;


public slots:
    void recvImg(QImage);
    void recvImgOne(QImage);
    void shout();
    void saveImg();
//    void change(int);
};

#endif // MAINWINDOW_H
