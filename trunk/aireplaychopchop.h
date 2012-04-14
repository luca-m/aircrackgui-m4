#ifndef CHOPCHOP_H
#define CHOPCHOP_H

#include <QWidget>
#include <QProcess>
#include <QString>
#include <QTimer>
#include "DEFINES.h"
#include "GLOBALS.h"
#include "utils.h"
#include "aireplayarpreplay.h"
#include "attackchopchop.h"


namespace Ui {
    class chopChop;
}

class aireplayChopChop : public QWidget
{
    Q_OBJECT

public:
    explicit aireplayChopChop(QWidget *parent = 0);
    ~aireplayChopChop();

    enum STATUS {ATTACKING, STOPPED, OBTAINED};

    STATUS getStatus()const {return status;}
    QString getStatusQString();

signals:
    void toLog(QString com);
    void statusChanged(STATUS s);
    void succesfull(QString msg);
    void statusChanged(const QString s);

public slots:
    bool start(QString BSSID, QString MAC);
    void stop();
    void clearLog();
    void resetValues();

private slots:
    void toThisLog(QString com);
    void setStatus(STATUS s);
    void update(QString line);
    void injectPaquet();

private:
    Ui::chopChop *ui;
    aireplayArpReplay *arpReplay;
    attackChopChop *attack;
    STATUS status;
    QString lastBSSID;
    QString lastMAC;
    QString lastDecryptedCap;
    QString lastXor;
};

#endif // CHOPCHOP_H
