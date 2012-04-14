#include "aireplaybroadcast.h"
#include "ui_aireplaybroadcast.h"

aireplayBroadcast::aireplayBroadcast(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::aireplayBroadcast)
{
    logThread::addLog("Constructor of broadcast attack GUI", logInfo::MAIN);

    attack = new attackBroadcast();

    ui->setupUi(this);
    connect(attack, SIGNAL(processOutput(QString)), this, SLOT(update(QString)));
    connect(this->ui->pushButtonStop, SIGNAL(clicked()), this, SLOT(stop()));
    connect(this->ui->pushButtonLog, SIGNAL(clicked()), this, SLOT(clearLog()));


    this->setStatus(STOPPED);

}

aireplayBroadcast::~aireplayBroadcast()
{
    logThread::addLog("Destructor of broadcast attack GUI", logInfo::MAIN);
    delete ui;
    delete attack;
}

void aireplayBroadcast::resetValues(){
    this->ui->spinBoxReading->setValue(0);
    this->ui->spinBoxSended->setValue(0);
    this->ui->spinBoxSending->setValue(0);
}

void aireplayBroadcast::clearLog(){
    utils::customClearLog(this->ui->textEdit, "Broadcast Attack Log");
}

void aireplayBroadcast::toThisLog(QString com){
    this->ui->textEdit->append(com);
}


QString aireplayBroadcast::getStatusQString()
{
    switch (status) {
        case STOPPED:
            return "STOPPED";
            break;
        case READING:
            return "READING";
            break;
        case SENDING:
            return "SENDING";
            break;
        default:
            return "UNKNOWN";
            break;
    }
}

void aireplayBroadcast::setStatus(STATUS s){
    {status = s;}
    switch (s) {
        case STOPPED:
            this->ui->lineEditStatus->setText("STOPPED");
            utils::setBackgroundColor(this->ui->lineEditStatus, utils::RED);
            break;
        case READING:
            this->ui->lineEditStatus->setText("READING PACKETS...");
            utils::setBackgroundColor(this->ui->lineEditStatus, utils::YELLOW);
            break;
        case SENDING:
            utils::setBackgroundColor(this->ui->lineEditStatus, utils::GREEN);
            this->ui->lineEditStatus->setText("SENDING PACKETS...");
            emit succesfull("BROADCAST ATTACK SUCCESFULL! RE-INYECTING!");
            break;
    }

    logThread::addLog("Broadcast: Status changes to " + getStatusQString(), logInfo::MAIN);
    emit statusChanged(getStatusQString());

}

bool aireplayBroadcast::start(const QString &BSSID, QString MAC)
{
    logThread::addLog(QString("Broadcast: Starting attack with BSSID=%1, MAC=%2").arg(BSSID).arg(MAC), logInfo::MAIN);

    //set working directory
    if (!QFile::exists(BROADCAST_FOLDER + BSSID))
        QDir::current().mkdir(BROADCAST_FOLDER + BSSID);

    attack->getProcess()->setWorkingDirectory(BROADCAST_FOLDER + BSSID);

    const bool ok = attack->start(BSSID, MAC);

    if (ok)
        this->setStatus(READING);

    return ok;

}

void aireplayBroadcast::stop(){

    logThread::addLog("Broadcast: Stopping", logInfo::MAIN);

    if (attack->isRunning()) {
        attack->stop();
        toThisLog("Process stopped");
    }

    this->setStatus(STOPPED);
}

void aireplayBroadcast::update(QString info){
    info.remove("\n");
    info.remove("\r");

    if (!info.isEmpty()) {
        if (info.contains("Size: ")){
            toThisLog(utils::htmlVerde("We found a packet!!!"));
            toThisLog(utils::htmlVerde("Sending..."));
            toThisLog(info);
            this->setStatus(SENDING);
        }


        else if (info.contains("Read")) {
                this->setStatus(READING);
                info.remove(0, 5);
                const int index2 = info.indexOf("packets");
                info.remove(index2, info.size()-index2);
                this->ui->spinBoxReading->setValue(info.toInt());
        }

        else if (info.contains("Sent")) {
                info.remove("Sent ");
                const int index = info.indexOf("pa");
                this->ui->spinBoxSended->setValue(info.remove(index, info.size()-index).toInt());
                this->ui->spinBoxSending->setValue(GLOBALS::SEND_RATE);

        }

        else
            toThisLog(info);
    }

}
