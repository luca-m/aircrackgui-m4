#ifndef BASEATTACK_H
#define BASEATTACK_H

#include <QObject>
#include <QProcess>
#include <QThread>
#include "DEFINES.h"
#include "utils.h"

/*
  This is the clase base of all attacks. THE GOD CLASS :).
*/

class baseAttack : public QObject
{
    Q_OBJECT

public:
    explicit baseAttack(QObject *parent = 0);
    ~baseAttack();

    bool isRunning();
    QProcess* getProcess(){return process;}
    void sendSignalToProcess(int signal);
    bool stop();

signals:
    void processOutput(QString line);
    void started();
    void finished(int code);

protected:
    bool startCommand(const QString &command);
    QProcess *process;

private slots:
    void outputReceived();
};

#endif // BASEATTACK_H
