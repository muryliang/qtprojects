#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "debug.h"

MainWindow::MainWindow(QString confpath, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    handle = new PkgHandle();
    if (handle->loadConfig(confpath.toStdString()) != 0) {
        qDebug("can not load config");
    }
    handle->init();
    connect(ui->pushButton, SIGNAL(clicked(bool)), this, SLOT(handleinstall()));
    connect(ui->pushButton_2, SIGNAL(clicked(bool)), this, SLOT(handleuninstall()));
}

MainWindow::~MainWindow()
{
    delete ui;
    handle->uninit();
}

void MainWindow::handleinstall() {
    pr_info("begin handle install");
    std::vector<std::string> installlist;
    installlist.push_back("one-1.2.tgz");
    installlist.push_back("two-1.1.tgz");
    handle->install(installlist);
}

void MainWindow::handleuninstall() {
    pr_info("begin handle uninstall");
    std::vector<std::string> uninstalllist;
    uninstalllist.push_back("one");
    uninstalllist.push_back("two");
    handle->uninstall(uninstalllist);
}

