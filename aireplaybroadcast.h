#ifndef AIREPLAYBROADCAST_H
#define AIREPLAYBROADCAST_H

#include <QWidget>
#include <QProcess>
#include <QString>
#include <QTimer>
#include "DEFINES.h"
#include "GLOBALS.h"
#include "utils.h"
#include "attackbroadcast.h"

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
    QString getStatusQString();

signals:
    void toLog(QString com);
    void statusChanged(STATUS s);
    void succesfull(QString msg);
    void statusChanged(const QString s);

public slots:
    bool start(const QString &BSSID, QString MAC);
    void stop();
    void clearLog();
    void resetValues();

private slots:
    void toThisLog(QString com);
    void setStatus(STATUS s);
    void update(QString line);

private:
    Ui::aireplayBroadcast *ui;
    attackBroadcast *attack;
    STATUS status; //READING, SENDING, STOPPED
};

#endif // AIREPLAYBROADCAST_H
