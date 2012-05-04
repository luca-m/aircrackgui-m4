#ifndef AIRODUMP_H
#define AIRODUMP_H

#include <QMainWindow>
#include <QLayout>
#include <stdio.h>
#include <QTimer>
#include <QProcess>
#include <QString>
#include <QFileDialog>
#include <QAbstractItemView>
#include <QComboBox>
#include <QProgressBar>
#include <QMessageBox>
#include <QMutex>
#include <time.h>
#include <QDesktopServices>
#include <QUrl>
#include "infoessid.h"
#include "infoconnectionbssid.h"
#include "DEFINES.h"
#include "GLOBALS.h"
#include "historicalnetwork.h"


namespace Ui {
    class Airodump;
}

class Airodump : public QWidget
{
    Q_OBJECT

public:
    explicit Airodump(QWidget *parent = 0);
    ~Airodump();
    void setHistoricalNetwork(historicalNetwork *p){histAux = p;}

    enum STATUS {STOPPED, MONITORING, CAPTURING};

    //get rows selected
    infoESSID* getSelectedInfoESSID();
    infoConnectionBssid* getSelectedInfoConnectionBssid();


signals:
    //to emit a command or error
     void toLog(QString com);
     //attack deauth
     void doAttackDeauth(QString Bssid, QString MAC);
     //attack auth
     void doAttackAuth(QString Bssid, QString MAC);
     //attack broadcast
     void doAttackBroadcast(QString BSSID, QString MAC);
     //attack ARP replay
     void doAttackArpReplay(QString BSSID, QString MAC);
     //attack interactive ARP
     void doAttackInteractiveARP(QString BSSID, QString MAC, QString cap);
     //aircrack
     void doAircrack(QString BSSID);
     void doAttackReaver(QString BSSID);
     //frag
     void doAttackFragmentation(QString BSSID, QString MAC);
     //chop
     void doAttackChopChop(QString BSSID, QString MAC);
     //status changed
     void statusChanged(STATUS newStatus);
     //new station
     void stationAppear(infoConnectionBssid infoC);
     //new BSSID
     void BSSIDAppear(infoESSID infoE);
     //focus
     void focus(QWidget *w);
     //stop all
     void doStopAllAttacks();
     //
     void statusInform(QString msg);
     void injectionRateChanged(const int rate);

public slots:
     //clear the tables
    void clearTables();
    void stop();
    //sort tables
    void sort();
    //select stations associated in the bottom table when we select a bssid in the top table
    void selectStationsAssociated();
    //new stations appear below
    void newStationAppear(infoConnectionBssid infoC);
    //new bssid
    void newBSSIDAppear(infoESSID infoE);
    //receive the order to do attack deauth
    void attackDeauth();
    //receive the order to do attack auth
    void attackAuth();
    //receive the order to do attack broadcast
    void attackBroadcast();
    //receive de order to do arp replay
    void attackArpReplay();
    //receive the order to crack
    void aircrack();
    //capture start
    void capture();
    //receive order to do interactive attack
    void attackInteractiveARP();
    //receive order to do frag attack
    void attackFragmetation();
    //receive order to do chop attack
    void attackChopChop();
    //receive the order to do reaver attack
    void attackReaver();
    //receive the order to do m4 attack
    void attackM4();
    //receive de order to do all attacks
    void allAttacks();
    //receive the order to stop all
    void stopAllAttacks();
    //set MAIN interface
    void setMainInterface(QString interface);
    void openDonate();
    // update status
    void updateAuthStatus(const QString &status);
    void updateDeauthStatus(const QString &status);
    void updateBroadcastStatus(const QString &status);
    void updateChopChopStatus(const QString &status);
    void updateFragmentationStatus(const QString &status);
    void updateReaverStatus(const QString &status);
    void updateARPReplayStatus(const QString &status);
    void updateAircrackStatus(const QString &status);

private slots:
    void update();
    //normal start
    void start();
    //start airodump with flags indicated in interface (channel, filter encrypt...)
    void startMonitor(bool capture = false);
    void setStatus(STATUS s);
    //show correct options depends of the status
    void enableCorrectOptions(STATUS status);
    //show more info
    void showMoreInfo();
    void reloadInterfaces();
    void availableLater();
    void restart();

private:
    //clear vectors of infoESSIDV and infoConnectionBssidV
    void clearInfoVectors();
    bool openFileM4();
    //convert a table of info essid. After read the file buffer, we call this function to store info in the vectors infoESSIDV, infoConnectionBssidV.
    int convertInfo();
    //insert a essid row info into a table of essid info
    bool insertRow(const infoESSID &info, QTableWidget *table);
    //the same to other table of infoConnectionBssid
    bool insertRow(const infoConnectionBssid &infoC,  QTableWidget *table);
    //get rows
    infoESSID getRowInfoESSID(int row);
    infoConnectionBssid getRowInfoConnectionBssid(int row);
    infoESSIDV infoE;
    infoConnectionBssidV infoC;
    QProcess *process;
    //file buffer where we can read data to print in the tables
    QFile fileM4;
    historicalNetwork *histAux;
    STATUS status;
    Ui::Airodump *ui;

};

#endif // AIRODUMP_H
