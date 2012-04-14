#include <QtGui/QApplication>
#include "mainwindow.h"
#include "qprocess.h"
#include "airodump.h"
#include "airmon.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    if (argc == 2)
        if (QString(argv[1]) == "-d") {
            fprintf(stderr, "-------> Mode debug active\n");
            GLOBALS::debug = true;
        }


    MainWindow w;
    w.showMaximized();
    return app.exec();


}
