#ifndef AIREPLAYARPREPLAY_H
#define AIREPLAYARPREPLAY_H

#include <QWidget>
#include <QWidget>
#include "QProcess"
#include "qstring.h"
#include "DEFINES.h"
#include "GLOBALS.h"
#include "utils.h"
#include "QTimer"
#include "debug.h"

namespace Ui {
    class aireplayArpReplay;
}

class aireplayArpReplay : public QWidget
{
    Q_OBJECT



public:
    explicit aireplayArpReplay(QWidget *parent = 0);
    ~aireplayArpReplay();

    void toThisLog(QString com);

    enum STATUS {READING, SENDING, STOPPED};
    STATUS getStatus()const {return status;}

private:
    Ui::aireplayArpReplay *ui;
    QProcess process;
    STATUS status; //READING, SENDING, STOPPED

public slots:
    void start(QString BSSID, QString MAC, QString cap = "");
    void stop();
    void clearLog();
    void resetValues();

private slots:
    void update();
    void setStatus(STATUS s);

signals:
    void toLog(QString com);
    void statusChanged(STATUS s);
    void succesfull(QString msg);
};

#endif // AIREPLAYARPREPLAY_H
