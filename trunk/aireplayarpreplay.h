#ifndef AIREPLAYARPREPLAY_H
#define AIREPLAYARPREPLAY_H

#include <QWidget>
#include <QProcess>
#include <QString>
#include <QTimer>
#include "DEFINES.h"
#include "GLOBALS.h"
#include "utils.h"
#include "attackarpreplay.h"

namespace Ui {
    class aireplayArpReplay;
}

class aireplayArpReplay : public QWidget
{
    Q_OBJECT

public:
    explicit aireplayArpReplay(QWidget *parent = 0);
    ~aireplayArpReplay();

    enum STATUS {READING, SENDING, STOPPED};

    STATUS getStatus()const {return status;}
    QString getStatusQString();


signals:
    void toLog(QString com);
    void statusChanged(STATUS s);
    void statusChanged(const QString s);
    void succesfull(QString msg);

public slots:
    bool start(const QString &BSSID, QString MAC, QString cap = "");
    void stop();
    void clearLog();
    void resetValues();
    void toThisLog(const QString &com);

private slots:
    void update(QString info);
    void setStatus(STATUS s);

private:
    Ui::aireplayArpReplay *ui;
    STATUS status; //READING, SENDING, STOPPED
    attackArpReplay *attack;
};

#endif // AIREPLAYARPREPLAY_H
