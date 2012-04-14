#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLayout>
#include <stdio.h>
#include <QTimer>
#include <QSplashScreen>
#include <QPixmap>
#include "airodump.h"
#include "airmon.h"
#include "setinterface.h"
#include "GLOBALS.h"
#include "aireplaydeauth.h"
#include "aireplayauth.h"
#include "aireplaybroadcast.h"
#include "macchanger.h"
#include "utils.h"
#include "aircrack.h"
#include "options.h"
#include "aireplayarpreplay.h"
#include "historicalnetwork.h"
#include "aireplayfragmentation.h"
#include "aireplaychopchop.h"
#include "logthread.h"
#include "reaver.h"
#include "updater.h"

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

signals:
    void toStatusBar(QString com);


public slots:
    //add text to log
    void addLog(QString);
    void launchDeauth(const QString &BSSID, QString MAC);
    void launchAuth(const QString &BSSID, QString MAC);
    void launchBroadcast(const QString &BSSID, QString MAC);
    void launchArpReplay(const QString &BSSID, QString MAC, QString cap = ""); //magic of generic things :).
                                                                        //here implement arp replay & interactive
    void launchFragmentation(const QString &BSSID, QString MAC);
    void launchChopChop(const QString &BSSID, QString MAC);
    void launchAircrack(const QString &BSSID);
    void launchReaver(const QString &BSSID);
    //qt help
    void aboutQt();
    //this program about
    void aboutThisProgram();
    //about aircrack
    void aboutAircrack();
    //report bug
    void reportBug();
    //stop all processes
    void stopAll();
    //stop all attacks
    void stopAllAttacks();
    //focus tab
    void focusTab(QWidget *window);

private slots:
    void openLogs();
private:
    //widget's
    Airodump *airodump;
    airmon *airm;
    aireplayDeauth *aireDeauth;
    aireplayAuth *aireAuth;
    aireplayBroadcast *aireBroad;
    Aircrack *aircrack;
    Macchanger *macchanger;
    Options *options;
    aireplayArpReplay *aireARP;
    historicalNetwork *hist;
    aireplayFragmentation *frag;
    aireplayChopChop *chop;
    reaver *reav;
    updater *updat;

    void init();

    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
