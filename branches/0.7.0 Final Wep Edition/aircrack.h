#ifndef AIRCRACK_H
#define AIRCRACK_H

#include <QWidget>
#include "qprocess.h"
#include "DEFINES.h"
#include "GLOBALS.h"
#include "utils.h"
#include "QTimer"
#include "debug.h"

namespace Ui {
    class Aircrack;
}

class Aircrack : public QWidget
{
    Q_OBJECT



public:
    explicit Aircrack(QWidget *parent = 0);
    ~Aircrack();
    enum STATUS {CRACKING, KEY_OBTAINED, STOPPED, OPENNING};
    STATUS getStatus() const {return status;}

public slots:
    void start(QString BSSID);
    void stop();
    void clearLog();
    void resetValues();


signals:
    void toLog(QString command);
    void statusChanged(STATUS s);


private slots:
    void toThisLog(QString com);
    void update();
    void setStatus(STATUS s);

private:
    Ui::Aircrack *ui;
    QProcess process;
    //variable that stores last BSSID used in cracking
    QString lastBSSID;
    STATUS status;
};

#endif // AIRCRACK_H
