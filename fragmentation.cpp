#include "fragmentation.h"
#include "ui_fragmentation.h"

fragmentation::fragmentation(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::fragmentation)
{
    debug::add("Constructor of aireplay -5 [Fragmentation] object " + QString::number(this->winId()));

    ui->setupUi(this);
    connect(this->ui->pushButtonStop, SIGNAL(clicked()), this, SLOT(stop()));
    connect(this->ui->pushButtonLog, SIGNAL(clicked()), this, SLOT(clearLog()));

    process.setProcessChannelMode(QProcess::MergedChannels);
    connect(&process, SIGNAL(readyRead()), this, SLOT(update()));


    this->arpReplay = NULL;


    this->setStatus(STOPPED);
}

fragmentation::~fragmentation()
{
    debug::add("Destructor of aireplay -5 [Fragmentation] object " + QString::number(this->winId()));

    delete ui;
    if (arpReplay)
        delete arpReplay;
}

void fragmentation::resetValues(){
    this->ui->spinBoxReading->setValue(0);
}

void fragmentation::clearLog(){
    utils::customClearLog(this->ui->textEdit, "Fragmentation Attack Log");
}

void fragmentation::toThisLog(QString com){
    this->ui->textEdit->append(com);
}

void fragmentation::setStatus(STATUS s){
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

}


void fragmentation::injectPaquet(){

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

void fragmentation::start(QString BSSID, QString MAC){

    if (!utils::validMAC(BSSID)){
        utils::mostrarError("Invalid BSSID");
        return;
    }

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
    if (!QFile::exists(FRAG_FOLDER + BSSID))
        system( QString("mkdir " + FRAG_FOLDER + BSSID).toLatin1().data() );

    //set working directory
    process.setWorkingDirectory(FRAG_FOLDER + BSSID);

    const QString command = "../../" + AIREPLAY_COM +
            " -5 " +
            " -m " + QString::number(GLOBALS::FRAG_MIN_PACKET) +
            " -n " + QString::number(GLOBALS::FRAG_MAX_PACKET) +
            " -b " + BSSID +
            MAC +
            " " + GLOBALS::INTERFACE;

    //if process is already running, we have to stop it
    stop();
    process.start(command);
    debug::add("Starting aireplay -5 [Fragmentation] process " + QString::number(process.pid()));

    emit toLog(command);

    if (!process.waitForStarted(WAIT_FOR_PROCESS)){
        utils::mostrarError("Error al intentar ejecutar " + command);
        return;
    }

    toThisLog("Reading packets to do Frag Attack...");
    toThisLog("If reading take a long time, you can try to increase the maximum packet filter in options");
    this->setStatus(ATTACKING);
    //reset values
    this->resetValues();
}

void fragmentation::stop(){
    if (process.state() == QProcess::Running) {
        debug::add("Stopping aireplay -5 [Fragmentation] process " + QString::number(process.pid()));

        utils::killProcess(process.pid());
        if (!process.waitForFinished(WAIT_FOR_PROCESS))
            utils::mostrarError("Imposible to terminate attack fragmentation process");
        toThisLog("Process stopped");
    }

    //stopping arpreplay step 2 also
    if (arpReplay)
        if (arpReplay->getStatus() != aireplayArpReplay::STOPPED)
            arpReplay->stop();

    this->setStatus(STOPPED);
}

void fragmentation::update(){
    static QString info;


    do {
        info = process.readLine(); //read line to line
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
            }

            //obtained
            else if (info.contains("Now you can build")) {
                toThisLog(utils::htmlVerde("Building and preparing arp request to be injected..."));
                this->setStatus(OBTAINED);
                //LAUNCH INJECT!
                QTimer::singleShot(1000, this, SLOT(injectPaquet()));
            }

            else
                toThisLog(info);
        }


    } while (!info.isEmpty());

}
