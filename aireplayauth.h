#ifndef AIREPLAYAUTH_H
#define AIREPLAYAUTH_H

#include <QWidget>
#include "utils.h"
#include "DEFINES.h"
#include "QString"
#include "GLOBALS.h"
#include "QTimer"
#include "qfile.h"
#include "debug.h"

namespace Ui {
    class aireplayAuth;
}

class aireplayAuth : public QWidget
{
    Q_OBJECT

private:
    Ui::aireplayAuth *ui;
    QProcess process;


public:
    explicit aireplayAuth(QWidget *parent = 0);
    ~aireplayAuth();
    enum STATUS {AUTHENTICATED, AUTHENTICATING, STOPPED};
    STATUS status;
    STATUS getStatus()const {return status;}

public slots:
    void start(QString BSSID, int count, QString mac, bool fromOutside = false); //slot to start DIRECTLY with theses values
    void stop();
    void clearLog();

private slots:
    void update();
    void toThisLog(QString com);
    void start(); //slot to start with the interface values
    void setStatus(STATUS s);


signals:
    void toLog(QString command);
    void statusChanged(STATUS s);
    void succesfull(QString msg);
};

#endif // AIREPLAYAUTH_H
