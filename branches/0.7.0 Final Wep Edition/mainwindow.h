#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "qlayout.h"
#include "stdio.h"
#include "QTimer"
#include "airodump.h"
#include "airmon.h"
#include "setinterface.h"
#include "GLOBALS.h"
#include "aireplaydeauth.h"
#include "aireplayauth.h"
#include "aireplaybroadcast.h"
#include "macchanger.h"
#include "aircrack.h"
#include "options.h"
#include "aireplayarpreplay.h"
#include "historicalnetwork.h"
#include "updater.h"
#include "qsplashscreen.h"
#include "qpixmap.h"
#include "fragmentation.h"
#include "chopchop.h"
#include "debug.h"

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    //set the global interface to work with
    void setIface(QString iface);
    //load interfaces in the option set interface
    void loadInterfaces();
    //add text to log
    void addLog(QString);
    void launchDeauth(QString BSSID, int count, QString MAC);
    void launchAuth(QString BSSID, int count, QString MAC);
    void launchBroadcast(QString BSSID, QString MAC);
    void launchArpReplay(QString BSSID, QString MAC, QString cap = ""); //magic of generic things :).
                                                                        //here implement arp replay & interactive

    void launchFragmentation(QString BSSID, QString MAC);
    void launchChopChop(QString BSSID, QString MAC);
    void launchAircrack(QString BSSID);
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

signals:
    void toStatusBar(QString com);

private:
    //widget airodump
    Airodump *airodump;
    //widget airmon
    airmon *airm;
    //widget setInterface
    setInterface *WsetIface;
    //widget aireplay death
    aireplayDeauth *aireDeauth;
    //widget aireplayAuth
    aireplayAuth *aireAuth;
    //attack bropadcast
    aireplayBroadcast *aireBroad;
    //aircrack
    Aircrack *aircrack;
    //macchanger
    Macchanger *macchanger;
    //options
    Options *options;
    //arp replay
    aireplayArpReplay *aireARP;
    historicalNetwork *hist;
    updater *update;
    fragmentation *frag;
    chopChop *chop;

    QTimer autoUpdates;


    void init();

    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
