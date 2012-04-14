#include "aireplayarpreplay.h"
#include "ui_aireplayarpreplay.h"

aireplayArpReplay::aireplayArpReplay(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::aireplayArpReplay)
{
    logThread::addLog("Constructor of ARP replay GUI", logInfo::MAIN);

    attack = new attackArpReplay();

    ui->setupUi(this);
    connect(attack, SIGNAL(processOutput(QString)), this, SLOT(update(QString)));
    connect(this->ui->pushButtonStop, SIGNAL(clicked()), this, SLOT(stop()));
    connect(this->ui->pushButtonLog, SIGNAL(clicked()), this, SLOT(clearLog()));

    this->setStatus(STOPPED);

}

aireplayArpReplay::~aireplayArpReplay()
{
    logThread::addLog("Destructor of ARP replay GUI", logInfo::MAIN);
    delete ui;
    delete attack;
}


QString aireplayArpReplay::getStatusQString()
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

void aireplayArpReplay::resetValues(){
    this->ui->spinBoxReading->setValue(0);
    this->ui->spinBoxSended->setValue(0);
    this->ui->spinBoxSending->setValue(0);
    this->ui->spinBoxArpRequests->setValue(0);
}

void aireplayArpReplay::clearLog(){
    utils::customClearLog(this->ui->textEdit, "Aireplay ARP Replay Log");
}

void aireplayArpReplay::toThisLog(const QString &com){
    this->ui->textEdit->append(com);
}

void aireplayArpReplay::setStatus(STATUS s){
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
        this->ui->lineEditStatus->setText("REINJECTING ARP PACKETS...");
        emit succesfull("ARP REPLAY ATTACK SUCCESFULL! RE-INYECTING!");
        break;
    }

    logThread::addLog("ARP Replay: Status changes to " + getStatusQString(), logInfo::MAIN);
    emit statusChanged(getStatusQString());

}

bool aireplayArpReplay::start(const QString &BSSID, QString MAC, QString cap){
    logThread::addLog(QString("ARP Replay: Starting attack with BSSID=%1, MAC=%2, cap=%3")
                      .arg(BSSID).arg(MAC).arg(cap), logInfo::MAIN);

    //checking if the folder BSSID exists. If not, we create it to store there caps
    if (!QFile::exists(ARP_FOLDER + BSSID))
        QDir::current().mkdir(ARP_FOLDER + BSSID);

    //set working directory
    attack->getProcess()->setWorkingDirectory(ARP_FOLDER + BSSID);

    const bool ok = attack->start(BSSID, MAC, cap);

    if (ok)
        this->setStatus(READING);

    return ok;

}

void aireplayArpReplay::stop(){

    logThread::addLog("ARP Replay: Stopping", logInfo::MAIN);

    if (attack->isRunning()) {
        attack->stop();
        toThisLog("Process stopped");
    }

    this->setStatus(STOPPED);
}

void aireplayArpReplay::update(QString info){
    static int arpAux;
    info.remove("\n");
    info.remove("\r");

    if (!info.isEmpty()) {

        //cheking errors
        if (info.contains("failed") || info.contains("select failed") || info.contains("but the AP uses")){
            toThisLog(utils::htmlRojo(info));
            this->ui->pushButtonStop->click();
            return;
        }
        //ckecking succesful.

        //where we store the arp cap
        else if (info.contains("Saving"))
            toThisLog(utils::htmlVerde(info + " (" + ARP_FOLDER + ")"));

        //skipping 'You should also start airodump-ng to capture replies.'
        else if (info.contains("You should"))
            ;

        //Read 1 packets (got 0 ARP requests and 0 ACKs), sent 0 packets...(0 pps)
        else if (info.contains("Read")){
            //drop to reading
            this->ui->spinBoxReading->setValue(utils::dropNumber(info));
            //drop ACKS
            this->ui->spinBoxACK->setValue(utils::dropNumber(info.split("ARP").at(1)));
            //drop ARP
            arpAux = utils::dropNumber(info.split('(').at(1));
            //WE GOT ANY ARP???
            if (arpAux != 0){
                if (this->getStatus() != SENDING) {
                    this->setStatus(SENDING);
                    //drop pps (one time)
                    this->ui->spinBoxSending->setValue(GLOBALS::SEND_RATE);
                }

                this->ui->spinBoxArpRequests->setValue(arpAux);
                //drop sent
                this->ui->spinBoxSended->setValue(utils::dropNumber(info.split(',').at(1)));
            }

        }

        //normal information
        else
            toThisLog(info);
    }

}
