#ifndef AIREPLAYBROADCAST_H
#define AIREPLAYBROADCAST_H

#include <QWidget>
#include "QProcess"
#include "qstring.h"
#include "DEFINES.h"
#include "GLOBALS.h"
#include "utils.h"
#include "QTimer"
#include "debug.h"

namespace Ui {
    class aireplayBroadcast;
}

class aireplayBroadcast : public QWidget
{
    Q_OBJECT




public:
    explicit aireplayBroadcast(QWidget *parent = 0);
    ~aireplayBroadcast();
    enum STATUS {READING, SENDING, STOPPED};


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

signals:
    void toLog(QString com);
    void statusChanged(STATUS s);
    void succesfull(QString msg);

private:
    Ui::aireplayBroadcast *ui;
    QProcess process;
    STATUS status; //READING, SENDING, STOPPED
};

#endif // AIREPLAYBROADCAST_H
