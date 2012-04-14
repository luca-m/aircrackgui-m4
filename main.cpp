#include <QtGui/QApplication>
#include "mainwindow.h"
#include <QProcess>
#include "airodump.h"
#include "airmon.h"
#include "logthread.h"
#include <QDebug>

int main(int argc, char *argv[])
{

    QApplication app(argc, argv);

    logThread *log = new logThread(qApp);
    log->openLogFiles();
    log->start();

    MainWindow *w = new MainWindow();
    w->showMaximized();

    int retVal = app.exec();

    delete w;
    // wait 1s to complete logs
    sleep(1);

    log->terminate();
    log->exit(0);
    log->closeLogFiles();

    return retVal;
}
