#ifndef AIRODUMP_H
#define AIRODUMP_H

#include <QMainWindow>
#include "qlayout.h"
#include "stdio.h"
#include "QTimer"
#include "qprocess.h"
#include "qstring.h"
#include "infoessid.h"
#include "infoconnectionbssid.h"
#include "DEFINES.h"
#include "GLOBALS.h"
#include "qmutex.h"
#include "time.h"
#include "QProgressBar"
#include "qmessagebox.h"
#include "historicalnetwork.h"
#include "QFileDialog"
#include "QAbstractItemView"
#include "debug.h"




namespace Ui {
    class Airodump;
}

class Airodump : public QWidget
{
    Q_OBJECT

private:
    //info about BSSID's, first table
    infoESSIDV infoE;
    //info about connections in the BSSID'S, second table
    infoConnectionBssidV infoC;
    //process to execute airodump
    QProcess *process;
    //file buffer where we can read data to print in the tables
    QFile fileM4;
    //pointer to historical network
    historicalNetwork *histAux;
    //status
    enum STATUS {STOPPED, MONITORING, CAPTURING};
    STATUS status;
    //convert a table of info essid. After read the file buffer, we call this function to store info in the vectors infoESSIDV, infoConnectionBssidV.
    int convertInfo();
    //insert a essid row info into a table of essid info
    bool insertRow(const infoESSID &info, QTableWidget *table);
    //the same to other table of infoConnectionBssid
    bool insertRow(const infoConnectionBssid &infoC,  QTableWidget *table);

public:
    explicit Airodump(QWidget *parent = 0);
    ~Airodump();
    void setHistoricalNetwork(historicalNetwork *p){histAux = p;}

    //get rows selected
    infoESSID* getSelectedInfoESSID();
    infoConnectionBssid* getSelectedInfoConnectionBssid();


public slots:
    void stop();
    //test
    void test();
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
    //receive de order to do all attacks
    void allAttacks();
    //receive the order to stop all
    void stopAllAttacks();


private slots:
    void update();
    //normal start
    void start();
    //start airodump with flags indicated in interface (channel, filter encrypt...)
    void startMonitor(bool capture = false);
    //clear the tables
    void clearTables();
    void setStatus(STATUS s);
    //show correct options depends of the status
    void enableCorrectOptions(STATUS status);
    //show more info
    void showMoreInfo();


signals:
    //to emit a command or error
     void toLog(QString com);
     //attack deauth
     void doAttackDeauth(QString Bssid, int count, QString MAC);
     //attack auth
     void doAttackAuth(QString Bssid, int count, QString MAC);
     //attack broadcast
     void doAttackBroadcast(QString BSSID, QString MAC);
     //attack ARP replay
     void doAttackArpReplay(QString BSSID, QString MAC);
     //attack interactive ARP
     void doAttackInteractiveARP(QString BSSID, QString MAC, QString cap);
     //aircrack
     void doAircrack(QString BSSID);
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

private:
    Ui::Airodump *ui;
    //clear vectors of infoESSIDV and infoConnectionBssidV
    void clearInfoVectors();
    //open the M4 buffer
    bool openFileM4();
    //get rows
    infoESSID getRowInfoESSID(int row);
    infoConnectionBssid getRowInfoConnectionBssid(int row);




};

#endif // AIRODUMP_H
