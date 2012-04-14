#include "aircrack.h"
#include "ui_aircrack.h"

Aircrack::Aircrack(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Aircrack)
{

    logThread::addLog("Constructor of aircrack GUI", logInfo::MAIN);

    attack = new attackAircrack();
    ui->setupUi(this);
    connect(attack, SIGNAL(processOutput(QString)), this, SLOT(update(QString)));
    connect(this->ui->pushButtonStop, SIGNAL(clicked()), this, SLOT(stop()));
    connect(this->ui->pushButtonLog, SIGNAL(clicked()), this, SLOT(clearLog()));
    //SUPER FIX: maybe we found the key so fast that function update could not execute.
    connect(attack->getProcess(), SIGNAL(finished(int)), this, SLOT(update()));

    this->setStatus(STOPPED);

}

Aircrack::~Aircrack()
{
    logThread::addLog("Destructor of aircrack GUI", logInfo::MAIN);

    delete ui;
    delete attack;
}

void Aircrack::clearLog(){
    utils::customClearLog(this->ui->textEdit, "Aircrack Log");
}


QString Aircrack::getStatusQString()
{
    switch (status) {
        case STOPPED:
            return "STOPPED";
            break;
        case OPENNING:
            return "OPENNING";
            break;
        case CRACKING:
            return "CRACKING";
            break;
        case KEY_OBTAINED:
            return "KEY OBTAINED";
            break;
        default:
            return "UNKOWN";
            break;
    }
}


void Aircrack::start(const QString &BSSID){

    logThread::addLog("Aircrack: Starting cracking " + BSSID, logInfo::MAIN);

    //storing last BSSID
    lastBSSID = BSSID;

    const bool ok = attack->start(BSSID);

    if (ok)
        this->setStatus(CRACKING);


    //reset values
    this->resetValues();
}

void Aircrack::toThisLog(const QString &com){
    this->ui->textEdit->append(com);
}

void Aircrack::resetValues(){
    this->ui->spinBoxRead->setValue(0);
    this->ui->spinBoxTested->setValue(0);
}

void Aircrack::setStatus(const STATUS s){
    {status = s;}
    switch (s) {
    case STOPPED:
        this->ui->lineEditStatus->setText("STOPPED");
        utils::setBackgroundColor(this->ui->lineEditStatus, utils::RED);
        break;
    case CRACKING:
        this->ui->lineEditStatus->setText("CRACKING...");
        utils::setBackgroundColor(this->ui->lineEditStatus, utils::YELLOW);
        break;

    case OPENNING:
        this->ui->lineEditStatus->setText("OPENNING FILES...");
        utils::setBackgroundColor(this->ui->lineEditStatus, utils::GREEN);
        break;

    case KEY_OBTAINED:
        utils::setBackgroundColor(this->ui->lineEditStatus, utils::GREEN);
        this->ui->lineEditStatus->setText("KEY OBTAINED!");
        break;
    }

    logThread::addLog("Aircrack: Changes status to " + getStatusQString(), logInfo::MAIN);
    emit statusChanged(getStatusQString());

}
void Aircrack::stop(){

    logThread::addLog("Aircrack: Stopping", logInfo::MAIN);

    if (attack->isRunning()) {
        attack->stop();
        toThisLog("Process stopped");
    }

    if (this->getStatus() != KEY_OBTAINED)
        this->setStatus(STOPPED);

}

void Aircrack::update(QString info){
    static int index1;
    info.remove("\n");
    info.remove("\r");

    if (!info.isEmpty()) {

        //i dont know why aircrack does not start again himself when the IVS getting up. or not?
        if (info.contains("Opening"))
            this->setStatus(OPENNING);

        if ((index1 = info.indexOf("KEY")) != -1) {
            toThisLog(utils::htmlVerde(info.remove(0, index1)));
            this->setStatus(KEY_OBTAINED);
            this->ui->lineEditStatus->setText(info);
            //STORING KEY
            toThisLog(utils::htmlVerde("Storing key in " + CAPTURE_FOLDER + this->lastBSSID + ".key"));
            QFile f;
            f.setFileName(CAPTURE_FOLDER + this->lastBSSID + ".key");
            if (!f.open(QIODevice::WriteOnly)) {
                this->toThisLog(utils::htmlRojo("Error opening .key file"));
            }
            f.write(info.toLatin1());
            f.close();
            logThread::addLog(QString("Aircrack: KEY OBTAINED! (BSSID = %1, KEY=%2)").arg(this->lastBSSID).arg(info), logInfo::MAIN);
            //FIX: this message blocks the main thread. So we have to do in the last of
            //the storing key process.
            utils::mostrarMensaje(info, "We have had luck :)");

        }


        else if (info.contains("Starting PTW")) {
            this->ui->spinBoxRead->setValue(utils::dropNumber(info));
            toThisLog(info);
            this->setStatus(CRACKING);

        }

        else if (info.contains("Tested")) {
            //the line contain two values. Left one tested, right one read
            this->ui->spinBoxTested->setValue(utils::dropNumber(info.split("keys").at(0)));
        }


        else if (info.contains("Next try")){
            toThisLog(utils::htmlRojo("Failed. Restarting..."));
            //restart
            this->start(lastBSSID);
            logThread::addLog("Aircrack: Restarting", logInfo::MAIN);
        }



        else if (info.contains("No file"))
            this->setStatus(STOPPED);



        else
            toThisLog(info);

    }

/*
    //FORCE UPDATE. IF WE DONT FORCE, I DONT KNOW WHY THE ATTACK COULD BE PARALIZED
    if (this->getStatus() == CRACKING && attack->isRunning()) {
        ////debug::add("Forcing update in process aircrack " + QString::number(process.pid()));
        QTimer::singleShot(1000, this, SLOT(update()));
    }
    */
}
