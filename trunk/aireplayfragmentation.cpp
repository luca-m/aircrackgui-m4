#include "aireplayfragmentation.h"
#include "ui_fragmentation.h"
#include <QDebug>

aireplayFragmentation::aireplayFragmentation(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::fragmentation)
{
    logThread::addLog("Constructor of fragmentation attack GUI", logInfo::MAIN);

    attack = new attackFragmentation();
    ui->setupUi(this);
    connect(attack, SIGNAL(processOutput(QString)), this, SLOT(update(QString)));
    connect(this->ui->pushButtonStop, SIGNAL(clicked()), this, SLOT(stop()));
    connect(this->ui->pushButtonLog, SIGNAL(clicked()), this, SLOT(clearLog()));

    this->arpReplay = NULL;


    this->setStatus(STOPPED);
}

aireplayFragmentation::~aireplayFragmentation()
{
    logThread::addLog("Destructor of fragmentation attack GUI", logInfo::MAIN);
    delete ui;
    delete attack;
    if (arpReplay)
        delete arpReplay;
}

void aireplayFragmentation::resetValues(){
    this->ui->spinBoxReading->setValue(0);
}

void aireplayFragmentation::clearLog(){
    utils::customClearLog(this->ui->textEdit, "Fragmentation Attack Log");
}

void aireplayFragmentation::toThisLog(QString com){
    this->ui->textEdit->append(com);
}


QString aireplayFragmentation::getStatusQString()
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

void aireplayFragmentation::setStatus(STATUS s){
    {status = s;}
    switch (s) {
    case STOPPED:
        this->ui->lineEditStatus->setText("STOPPED");
        utils::setBackgroundColor(this->ui->lineEditStatus, utils::RED);
        break;
    case ATTACKING:
        this->ui->lineEditStatus->setText("ATTACKING...");
        utils::setBackgroundColor(this->ui->lineEditStatus, utils::YELLOW);
        break;
    case OBTAINED:
        this->ui->lineEditStatus->setText("OBTAINED");
        utils::setBackgroundColor(this->ui->lineEditStatus, utils::GREEN);
        break;
    }

    logThread::addLog("Fragmentation: Status changes to " + getStatusQString(), logInfo::MAIN);

    emit statusChanged(getStatusQString());

}


void aireplayFragmentation::injectPaquet(){

    logThread::addLog("Fragmentation: Injecting packet", logInfo::MAIN);

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

    //We have not any decrypted packet so we cannot find ipdest, so using broadcast
    //************************************************************************
    //by this method, we obtain 1 IV per each ARP Paquet reinjected
    //************************************************************************

    const QString arpForged =  utils::forgeARP(lastXor, lastBSSID, lastMAC,
                                               "255.255.255.255",
                                               "255.255.255.255");
    if (arpForged.isEmpty()) {
        toThisLog(utils::htmlRojo("arpForged Error File"));
        return;
    }

    toThisLog(utils::htmlVerde(QString("arpForged and stored in " + arpForged)));
    //start injecting
    arpReplay->start(lastBSSID, lastMAC, arpForged);
    toThisLog(utils::htmlVerde("INJECTING!!"));
    emit succesfull("FRAGMENTATION ATTACK SUCCESFULL! RE-INYECTING!");


}

bool aireplayFragmentation::start(QString BSSID, QString MAC){

    logThread::addLog(QString("Fragmentation: Starting attack with BSSID=%1, MAC=%2").arg(BSSID).arg(MAC), logInfo::MAIN);

    //checking if the folder BSSID exists. If not, we create it to store there caps
    if (!QFile::exists(FRAG_FOLDER + BSSID))
        QDir::current().mkdir(FRAG_FOLDER + BSSID);

    //set working directory
    attack->getProcess()->setWorkingDirectory(FRAG_FOLDER + BSSID);

    const bool ok = attack->start(BSSID, MAC);

    if (ok) {
        //storing BSSID & MAC
        this->lastBSSID = BSSID;
        this->lastMAC = MAC;
        this->setStatus(ATTACKING);
    }

    return ok;

}

void aireplayFragmentation::stop(){

    logThread::addLog("Fragmentation: Stopping", logInfo::MAIN);

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

void aireplayFragmentation::update(QString info){

    info.remove("\n");
    info.remove("\r");

    if (!info.isEmpty()) {

        if (info.contains("Read"))
            this->ui->spinBoxReading->setValue(utils::dropNumber(info));


        //arrive a packet
        else if (info.contains("Size: ")){
            toThisLog(utils::htmlVerde("We found a packet!!!"));
            toThisLog(info);
        }

        //normal error
        else if (info.contains("No answer") || info.contains("Not enough acks") ||
                 info.contains("trying another"))
            toThisLog(utils::htmlRojo(info));

        //good news
        else if (info.contains("RELAYED"))
            toThisLog(utils::htmlVerde(info));


        //Saving keystream in fragment-0511-221818.xor
        else if(info.contains("Saving keystream")) {
            toThisLog(utils::htmlVerde(info));
            //getting xor file
            lastXor = FRAG_FOLDER + lastBSSID + "/" + info.split(' ').at(3);
            //absolute path
            lastXor = QFileInfo(lastXor).absoluteFilePath();

            logThread::addLog("Fragmentation: Saving keystream to " + lastXor, logInfo::MAIN);
        }

        //obtained
        else if (info.contains("Now you can build")) {
            toThisLog(utils::htmlVerde("Building and preparing arp request to be injected..."));
            this->setStatus(OBTAINED);
            logThread::addLog("Fragmentation: ATTACK SUCCESFULL. Injecting in 1 second", logInfo::MAIN);
            //LAUNCH INJECT!
            QTimer::singleShot(1000, this, SLOT(injectPaquet()));
        }

        else
            toThisLog(info);
    }

}
