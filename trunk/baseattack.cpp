#include "baseattack.h"

baseAttack::baseAttack(QObject *parent) :
    QObject(parent)
{
    process = new QProcess();

    // error output = standard output (we dont know how aircrack handles errors)
    process->setProcessChannelMode(QProcess::MergedChannels);

    // connecting output process with output interface attack
    connect(process, SIGNAL(readyRead()), this, SLOT(outputReceived()));

    // "delegating signals"
    connect(process, SIGNAL(started()), this, SIGNAL(started()));
    connect(process, SIGNAL(finished(int)), this, SIGNAL(finished(int)));
}

baseAttack::~baseAttack()
{
    if (this->isRunning())
        this->stop();

    delete process;
}

void baseAttack::outputReceived()
{
    // line to line
    QString aux = process->readLine();

    while (!aux.isEmpty()) {
        emit processOutput(aux);
        aux = process->readLine();
    }
}

bool baseAttack::stop()
{
    this->process->terminate();
    if (!this->process->waitForFinished(1000)) {
        this->process->kill();
        if (!this->process->waitForFinished(1000))
            return false;
    }

    return true;
}


void baseAttack::sendSignalToProcess(int signal)
{
    system(QString("kill -%1 %2").arg(signal).arg(this->process->pid()).toLatin1().data());
}

bool baseAttack::startCommand(const QString &command)
{
    if (isRunning()) {
        process->terminate();
        process->kill();
        process->waitForFinished(5000);
    }

    process->start(command);
    if (!process->waitForStarted(5000)){
         utils::mostrarError("Error al intentar ejecutar " + command + " ." + process->readAll());
         return false;
    }

    return true;
}

bool baseAttack::isRunning()
{
    return process->state() != QProcess::NotRunning;
}
