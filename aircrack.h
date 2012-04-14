#ifndef AIRCRACK_H
#define AIRCRACK_H

#include <QWidget>
#include <QProcess>
#include <QTimer>
#include "DEFINES.h"
#include "GLOBALS.h"
#include "utils.h"
#include "attackaircrack.h"

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
    QString getStatusQString();

public slots:
    void start(const QString &BSSID);
    void stop();
    void clearLog();
    void resetValues();

signals:
    void toLog(const QString &command);
    void statusChanged(const STATUS s);
    void statusChanged(const QString s);

private slots:
    void toThisLog(const QString &com);
    void update(QString info);
    void setStatus(const STATUS s);

private:
    Ui::Aircrack *ui;
    //variable that stores last BSSID used in cracking
    QString lastBSSID;
    STATUS status;
    attackAircrack *attack;
};

#endif // AIRCRACK_H
