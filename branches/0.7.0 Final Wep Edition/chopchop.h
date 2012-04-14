#ifndef CHOPCHOP_H
#define CHOPCHOP_H

#include <QWidget>
#include "QProcess"
#include "qstring.h"
#include "DEFINES.h"
#include "GLOBALS.h"
#include "utils.h"
#include "QTimer"
#include "aireplayarpreplay.h"
#include "debug.h"

namespace Ui {
    class chopChop;
}

class chopChop : public QWidget
{
    Q_OBJECT

public:
    explicit chopChop(QWidget *parent = 0);
    ~chopChop();

    QProcess process;
    QString lastBSSID;
    QString lastMAC;
    QString lastDecryptedCap;
    QString lastXor;

    enum STATUS {ATTACKING, STOPPED, OBTAINED};
    STATUS status;


public:
     STATUS getStatus()const {return status;}


public slots:
    void start(QString BSSID, QString MAC);
    void stop();
    void clearLog();
    void resetValues();

private slots:
    void toThisLog(QString com);
    void setStatus(STATUS s);
    void update();
    void injectPaquet();


signals:
    void toLog(QString com);
    void statusChanged(STATUS s);
    void succesfull(QString msg);

private:
    Ui::chopChop *ui;
    aireplayArpReplay *arpReplay;

};

#endif // CHOPCHOP_H
