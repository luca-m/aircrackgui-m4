#ifndef AIREPLAYDEAUTH_H
#define AIREPLAYDEAUTH_H

#include <QWidget>
#include "utils.h"
#include "GLOBALS.h"
#include "DEFINES.h"
#include "QTimer"
#include "debug.h"

namespace Ui {
    class aireplayDeauth;
}

class aireplayDeauth : public QWidget
{
    Q_OBJECT

private:
    Ui::aireplayDeauth *ui;
    QProcess process;


public:
    explicit aireplayDeauth(QWidget *parent = 0);
    ~aireplayDeauth();
    enum STATUS {DEAUTHENTICATED, DEAUTHENTICATING, STOPPED};
    STATUS status;
    STATUS getStatus() const {return status;}

public slots:
    void start(QString BSSID, int count, QString destMa, bool fromOutside = false); //slot to start DIRECTLY with theses values
    void stop();
    void clearLog();

private slots:
    void update();
    void toThisLog(QString com);
    void start(); //slot to start with the interface values
    void setStatus(STATUS s);

signals:
    void toLog(QString command);
    void succesfull(QString msg);


};

#endif // AIREPLAYDEAUTH_H
