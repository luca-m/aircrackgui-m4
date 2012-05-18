#include "reaver.h"
#include "ui_reaver.h"
#include <QDebug>

reaver::reaver(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::reaver)
{
    logThread::addLog("Constructor of reaver attack GUI", logInfo::MAIN);

    ui->setupUi(this);
    attack = new attackReaver();

    this->setStatus(STOPPED);

    connect(attack, SIGNAL(processOutput(QString)), this, SLOT(update(QString)), Qt::QueuedConnection);
    connect(this->ui->pushButtonLog, SIGNAL(clicked()), this, SLOT(clearLog()));
    connect(this->ui->pushButtonStop, SIGNAL(clicked()), this, SLOT(stop()));
}

reaver::~reaver()
{
    logThread::addLog("Destructor of reaver attack GUI", logInfo::MAIN);
    delete ui;
    delete attack;
}


QString reaver::getStatusQString()
{
    switch (status) {
    case STOPPED:
        return "STOPPED";
        break;
    case RUNNING:
        return "RUNNING";
        break;
    case OBTAINED:
        return "OBTAINED";
        break;
    default:
        return "UNKOWN";
        break;
    }
}


void reaver::setStatus(const STATUS &s)
{
    {status = s;}
    switch (s) {
    case STOPPED:
        this->ui->lineEditStatus->setText("STOPPED");
        utils::setBackgroundColor(this->ui->lineEditStatus, utils::RED);
        break;
    case RUNNING:
        this->ui->lineEditStatus->setText("RUNNING");
        utils::setBackgroundColor(this->ui->lineEditStatus, utils::YELLOW);
        break;
    case OBTAINED:
        utils::setBackgroundColor(this->ui->lineEditStatus, utils::GREEN);
        this->ui->lineEditStatus->setText("OBTAINED");
        break;
    }

    emit statusChanged(getStatusQString());
    logThread::addLog("Reaver: Status changes to " + getStatusQString(), logInfo::MAIN);

}

void reaver::toThisLog(const QString &com)
{
    this->ui->textEdit->append(com);
}


void reaver::clearLog()
{
    utils::customClearLog(this->ui->textEdit, "Reaver Attack Log");
}

void reaver::update(QString info)
{
    info.remove("\n");
    info.remove("\r");

    if (info.contains("[!]"))
        toThisLog(utils::htmlRojo(info));

    else if (info.contains("cracked")) {
        toThisLog(info);
        this->setStatus(OBTAINED);
        //storing key
        QFile file(REAVER_FOLDER+lastBSSID+"/"+lastBSSID+".key");
        if (!file.open(QIODevice::WriteOnly)) {
            toThisLog(utils::htmlRojo("Impossible to store key in " + file.fileName()));
            return;
        }

        file.write(pinKey.toLatin1());
        toThisLog("info");
        file.close();
    }

    else if (info.contains("WPS PIN")) {
        pinKey.append(info+" ");
        toThisLog(info);
    }

    else if (info.contains("WPA PSK")) {
        pinKey.append(info+" ");
        toThisLog(info);
    }

    else if (info.contains("complete @")) {
        //taking %
        const int postIndex = info.indexOf('%');
        const int preIndex = info.indexOf(']');
        const int numSize = postIndex-preIndex;
        const float percentage = info.mid(preIndex+1,numSize-1).remove(" ").toFloat();
        this->ui->doubleSpinBoxComplete->setValue(percentage);
        //taking senconds/pin
        const int sPin = utils::dropNumber(info.split('(').at(1));
        this->ui->spinBoxPinSeconds->setValue(sPin);
        toThisLog(utils::htmlVerde(info));
    }

    else if (info.contains("Trying") || info.contains("complete") || info.contains("saved")
             || info.contains("Restored") || info.contains("AP SSID"))
        toThisLog(utils::htmlVerde(info));

    else
        toThisLog(info);
}

bool reaver::start(const QString &BSSID)
{
    logThread::addLog(QString("Reaver: Starting attack with BSSID=%1").arg(BSSID), logInfo::MAIN);

    //checking if the folder BSSID exists. If not, we create it to store there caps
    if (!QFile::exists(REAVER_FOLDER + BSSID))
        QDir::current().mkdir(REAVER_FOLDER + BSSID);

    //set working directory
    attack->getProcess()->setWorkingDirectory(REAVER_FOLDER + BSSID);
    //there are previous session?
    QString session = BSSID;
    session.remove(':').append(".wpc");
    bool ok;


    if (QFile::exists(REAVER_FOLDER + BSSID + "/" + session)) {
        logThread::addLog(QString("Reaver: restoring previous session: " + REAVER_FOLDER + BSSID + "/" + session), logInfo::MAIN);
        ok = attack->start(BSSID, " -s " + session);
    }
    else
        ok = attack->start(BSSID);

    if (ok) {
        this->setStatus(RUNNING);
        this->ui->labelBSSID->setText("BSSID: " + BSSID);
    }

    lastBSSID = BSSID;
    return ok;
}

bool reaver::stop()
{
    logThread::addLog("Reaver: Stopping sending signal CTRL-C", logInfo::MAIN);

    if (attack->isRunning()) {
        // to save session sending CTRL-C
        attack->sendSignalToProcess(2);
        sleep(1);
        attack->stop();
        toThisLog("Process stopped");
    }


    this->setStatus(STOPPED);
    return true;
}
