#include "logthread.h"
#include <QMetaObject>
#include <QDebug>
#include <QIODevice>


QList<logInfo> logThread::pendingLines;
QMutex logThread::mutex;
QFile logThread::mainLog;

logThread::logThread(QObject *parent) : QThread(parent)
{

}


void logThread::addLog(const QString &line, logInfo::TYPE_LOG type) {
    mutex.lock();
    if (mainLog.isOpen())
        pendingLines.append(logInfo(QDateTime::currentDateTime(), line, type));
    mutex.unlock();
}


void logThread::openLogFiles()
{
    if (!QFile::exists(LOGS_FOLDER))
        QDir::current().mkdir(LOGS_FOLDER);

    mainLog.setFileName(MAIN_LOG);
    if (!mainLog.open(QIODevice::Append | QIODevice::Unbuffered)) {
        utils::mostrarError("Impossible to open log files");
        return;
    }
    mainLog.write(QByteArray("\n*****************************************************************************\n"));
    mainLog.write((QDateTime::currentDateTime().toString("[dd/MM hh:mm:ss] ") + "Openning log. VERSION: ").toLatin1());
    mainLog.write(VERSION.toLatin1());
    mainLog.write(QByteArray("\n*****************************************************************************"));
}


void logThread::closeLogFiles()
{
    if (mainLog.isOpen()) {
        mainLog.write(QByteArray("\n*****************************************************************************\n"));
        mainLog.write((QDateTime::currentDateTime().toString("[dd/MM hh:mm:ss] ") + "Clossing log").toLatin1());
        mainLog.write(QByteArray("\n*****************************************************************************\n"));
        mainLog.close();
    }
}

void logThread::run()
{
    forever {
        mutex.lock();
        if (mainLog.isOpen()) {
            foreach (const logInfo &info, pendingLines)
                if (info.type == logInfo::MAIN)
                    mainLog.write(('\n' + info.time.toString("[dd/MM hh:mm:ss] ") + info.msg).toLatin1());

            pendingLines.clear();
        }
        mainLog.flush();
        mutex.unlock();
        msleep(250);
    }

}
