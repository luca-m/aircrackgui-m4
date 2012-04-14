#include "aireplaybroadcast.h"
#include "ui_aireplaybroadcast.h"

aireplayBroadcast::aireplayBroadcast(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::aireplayBroadcast)
{
    debug::add("Constructor of aireplay -2 [Broadcast] object " + QString::number(this->winId()));

    ui->setupUi(this);
    connect(this->ui->pushButtonStop, SIGNAL(clicked()), this, SLOT(stop()));
    connect(this->ui->pushButtonLog, SIGNAL(clicked()), this, SLOT(clearLog()));

    process.setProcessChannelMode(QProcess::MergedChannels);
    connect(&process, SIGNAL(readyRead()), this, SLOT(update()));

    this->setStatus(STOPPED);

}

aireplayBroadcast::~aireplayBroadcast()
{
    debug::add("Destructor of aireplay -2 [Broadcast] object " + QString::number(this->winId()));

    delete ui;
    if (process.state() == QProcess::Running) {
         utils::killProcess(process.pid());
         process.waitForFinished(WAIT_FOR_PROCESS);
         //to avoid recursion
         return;
     }
    toThisLog("Process stopped");
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

}

void aireplayBroadcast::start(QString BSSID, QString MAC){

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

    const QString command = "../../" + AIREPLAY_COM +
                            " -2 -p 0841 -c FF:FF:FF:FF:FF:FF " +
                            " -m " + QString::number(GLOBALS::BROADCAST_MIN_PACKET) +
                            " -n " + QString::number(GLOBALS::BROADCAST_MAX_PACKET) +
                            " -b " + BSSID +
                            MAC +
                            " -x" + QString::number(GLOBALS::SEND_RATE) + " " + GLOBALS::INTERFACE;

    //checking if the folder BSSID exists. If not, we create it to store there caps
    if (!QFile::exists(BROADCAST_FOLDER + BSSID))
        system( QString("mkdir " + BROADCAST_FOLDER + BSSID).toLatin1().data() );

    //set working directory
    process.setWorkingDirectory(BROADCAST_FOLDER + BSSID);

    //if process is already running, we have to stop it
    stop();
    process.start(command);
    debug::add("Starting aireplay -2 [Broadcast] process " + QString::number(process.pid()));

    emit toLog(command);

    if (!process.waitForStarted(WAIT_FOR_PROCESS)){
         utils::mostrarError("Error al intentar ejecutar " + command);
         return;
     }

    toThisLog("Trying to capture a packet to do broadcast attack...");
    this->setStatus(READING);
    //reset values
    this->resetValues();
}

void aireplayBroadcast::stop(){
    if (process.state() == QProcess::Running) {
        debug::add("Stopping aireplay -2 [Broadcast] process " + QString::number(process.pid()));

        utils::killProcess(process.pid());
        if (!process.waitForFinished(WAIT_FOR_PROCESS))
            utils::mostrarError("Imposible to terminate attack broadcast process");
        toThisLog("Process stopped");

    }
    this->setStatus(STOPPED);
}

void aireplayBroadcast::update(){
    static QString info;

    info = process.readAllStandardOutput(); //we only receive one order of packet. it is not necessary to read line to line.
    //info.remove("\n");
    info.remove("\r");

    if (!info.isEmpty()) {
        //arrive a packet
        if (info.contains("Size: ")){
            //print packet
            toThisLog(utils::htmlVerde("We found a packet!!!"));
            const int index = info.indexOf("Use this");
            toThisLog(info.remove(index, info.size()-index));
            toThisLog(utils::htmlVerde("Sending..."));
            this->setStatus(SENDING);
        }


        if (this->getStatus() == READING) {
            if (!info.contains("Read"))
                toThisLog(info);
            else {
                info.remove(0, 5);
                const int index2 = info.indexOf("packets");
                info.remove(index2, info.size()-index2);
                this->ui->spinBoxReading->setValue(info.toInt());
            }

        }

        else if (this->getStatus() == SENDING){
            info.remove("Sent ");
            const int index = info.indexOf("pa");
            this->ui->spinBoxSended->setValue(info.remove(index, info.size()-index).toInt());
            this->ui->spinBoxSending->setValue(GLOBALS::SEND_RATE);
        }
    }

}
