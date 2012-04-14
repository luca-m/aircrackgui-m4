#ifndef FRAGMENTATION_H
#define FRAGMENTATION_H

#include <QWidget>
#include <QProcess>
#include <QString>
#include <QTimer>
#include "DEFINES.h"
#include "GLOBALS.h"
#include "utils.h"
#include "aireplayarpreplay.h"
#include "attackfragmentation.h"

namespace Ui {
    class fragmentation;
}

class aireplayFragmentation : public QWidget
{
    Q_OBJECT

public:
    explicit aireplayFragmentation(QWidget *parent = 0);
    ~aireplayFragmentation();

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
    QString forge(QString xo);

    Ui::fragmentation *ui;
    aireplayArpReplay *arpReplay;
    attackFragmentation *attack;
    QString lastBSSID;
    QString lastMAC;
    QString lastXor;
    STATUS status;
};

#endif // FRAGMENTATION_H
