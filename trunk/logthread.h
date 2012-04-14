#ifndef LOGTHREAD_H
#define LOGTHREAD_H

#include <QThread>
#include <QString>
#include <QPair>
#include <QMutex>
#include <utils.h>
#include <QDateTime>
#include "DEFINES.h"

struct logInfo {
    enum TYPE_LOG {MAIN, COMMAND};
    logInfo(const QDateTime &t, const QString &m, const TYPE_LOG ty){time=t;msg=m;type=ty;}
    QDateTime time;
    QString msg;
    TYPE_LOG type;
};

class logThread : public QThread
{

public:
    explicit logThread(QObject *parent);

    static void openLogFiles();
    static void closeLogFiles();
    static void addLog(const QString &line, logInfo::TYPE_LOG type);

    void run();

private:
    static QFile mainLog;
    static QList<logInfo> pendingLines;
    static QMutex mutex;
};

#endif // LOGTHREAD_H
