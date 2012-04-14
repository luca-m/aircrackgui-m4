#ifndef FRAGMENTATION_H
#define FRAGMENTATION_H

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
    class fragmentation;
}

class fragmentation : public QWidget
{
    Q_OBJECT

public:
    explicit fragmentation(QWidget *parent = 0);
    ~fragmentation();

    QProcess process;
    QString lastBSSID;
    QString lastMAC;
    QString lastXor;

    enum STATUS {ATTACKING, STOPPED, OBTAINED};
    STATUS status;


private:
    QString forge(QString xo);


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
    Ui::fragmentation *ui;
    aireplayArpReplay *arpReplay;
};

#endif // FRAGMENTATION_H
