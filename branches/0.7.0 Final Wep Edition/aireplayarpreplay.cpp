#include "aireplayarpreplay.h"
#include "ui_aireplayarpreplay.h"

aireplayArpReplay::aireplayArpReplay(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::aireplayArpReplay)
{
    debug::add("Constructor of aireplay -3 [arpReplay] object " + QString::number(this->winId()));

    ui->setupUi(this);

    connect(this->ui->pushButtonStop, SIGNAL(clicked()), this, SLOT(stop()));
    connect(this->ui->pushButtonLog, SIGNAL(clicked()), this, SLOT(clearLog()));

    process.setProcessChannelMode(QProcess::MergedChannels);
    //experimental
    connect(&process, SIGNAL(readyRead()), this, SLOT(update()));

    this->setStatus(STOPPED);

}

aireplayArpReplay::~aireplayArpReplay()
{
    debug::add("Destructor of aireplay -3 [arpReplay] object " + QString::number(this->winId()));

    delete ui;
    if (process.state() == QProcess::Running) {
         utils::killProcess(process.pid());
         process.waitForFinished(WAIT_FOR_PROCESS);
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

void aireplayArpReplay::toThisLog(QString com){
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

}

void aireplayArpReplay::start(QString BSSID, QString MAC, QString cap){

    if (!utils::validMAC(BSSID)){
        utils::mostrarError("Invalid BSSID");
        return;
    }

    if (!MAC.isEmpty()) {
        if (!utils::validMAC(BSSID)){
                utils::mostrarError("Invalid MAC");
                return;
        }

        else
            MAC = " -h " + MAC; //add flag to mac
    }

    //checking if the folder BSSID exists. If not, we create it to store there caps
    if (!QFile::exists(ARP_FOLDER + BSSID))
        system( QString("mkdir " + ARP_FOLDER + BSSID).toLatin1().data() );

    //set working directory
    process.setWorkingDirectory(ARP_FOLDER + BSSID);

    //if cap is empty, there are no cap to use
    if (!cap.isEmpty())
        //cap have to be in source path of program
        cap = cap.prepend(" -r ");

    const QString command = "../../" + AIREPLAY_COM + " -3 -b "
                            + BSSID + MAC + cap + " -x" + QString::number(GLOBALS::SEND_RATE) + " " + GLOBALS::INTERFACE;

    //if process is already running, we have to stop it
    stop();
    process.start(command);
    debug::add("Starting aireplay -3 [arpReplay] process " + QString::number(process.pid()));
    emit toLog(command);

    if (!process.waitForStarted(WAIT_FOR_PROCESS)){
         utils::mostrarError("Error when try to execute " + command);
         return;
     }

    //toThisLog("Reading packets, waiting for an ARP request to be Reinjected...");
    this->setStatus(READING);
    //reset values
    this->resetValues();
}

void aireplayArpReplay::stop(){

    if (process.state() == QProcess::Running) {
        debug::add("Stopping process aireplay -3 " + QString::number(process.pid()));
        utils::killProcess(process.pid());
        process.kill();
        process.terminate();
        if (!process.waitForFinished(WAIT_FOR_PROCESS))
            utils::mostrarError("Imposible to terminate attack arp replay process");
        toThisLog("Process stopped");
        debug::add("Stopping aireplay -3 [arpReplay] process " + QString::number(process.pid()));

    }


    this->setStatus(STOPPED);
}

void aireplayArpReplay::update(){
    static QString info;
    static int arpAux;

    do {
        info = process.readLine();
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
                toThisLog(utils::htmlVerde(info));

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

    } while (!info.isEmpty());

}
