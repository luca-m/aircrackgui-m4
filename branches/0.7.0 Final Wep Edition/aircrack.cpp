#include "aircrack.h"
#include "ui_aircrack.h"

Aircrack::Aircrack(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Aircrack)
{
    debug::add("Constructor of aircrack object " + QString::number(this->winId()));

    ui->setupUi(this);
    connect(this->ui->pushButtonStop, SIGNAL(clicked()), this, SLOT(stop()));
    connect(this->ui->pushButtonLog, SIGNAL(clicked()), this, SLOT(clearLog()));
    process.setProcessChannelMode(QProcess::MergedChannels);
    connect(&process, SIGNAL(readyRead()), this, SLOT(update()));
    //SUPER FIX: maybe we found the key so fast that function update could not execute.
    connect(&process, SIGNAL(finished(int)), this, SLOT(update()));

    this->setStatus(STOPPED);

}

Aircrack::~Aircrack()
{
    debug::add("Destructor of aircrack object " + QString::number(this->winId()));

    delete ui;

    if (process.state() == QProcess::Running) {
        debug::add("Aircrack process " + QString::number(this->process.pid()) + " still running. Destructing...");
        utils::killProcess(process.pid());
        process.waitForFinished(WAIT_FOR_PROCESS);
    }
}

void Aircrack::clearLog(){
    utils::customClearLog(this->ui->textEdit, "Aircrack Log");
}

void Aircrack::start(QString BSSID){

    //if we launch many times since airodump attack auth, stop previous process
    if (process.state() == QProcess::Running) {
        this->ui->pushButtonStop->click();
        process.waitForFinished();
    }

    if (!utils::validMAC(BSSID)){
        utils::mostrarError("Invalid BSSID");
        this->ui->pushButtonStop->click();
        return;
    }


    //obtaining files
    QString files;
    int left = 0, right = 1;
    do {

        if (QFile::exists(CAPTURE_FOLDER + BSSID +"-"+QString::number(left)+QString::number(right)+".cap")) {
            files += BSSID + "-"+QString::number(left)+QString::number(right)+".cap" + " ";
            if (left == 9 && right == 9) {
                toThisLog(utils::htmlRojo("Maximum .cap files readed: 99. From X-99.cap to ahead we could not read data"));
                debug::add("There are more than 99 caps files. Maybe you should join them. Maximum caps files to read is 99");
            }
        }


        right++;
        if (right == 10) {
            right = 0;
            left++;
        }



    }   while (left != 10); //100 max

    const QString command = "../" + AIRCRACK_COM +
            " -b " + BSSID +
            " -f" + QString::number(GLOBALS::BRUTEFORCE_FACTOR) +
            + " " + files;
    process.setWorkingDirectory(CAPTURE_FOLDER);
    if (process.state() == QProcess::Running)
        stop();

    process.start(command);
    debug::add("Starting aircrack process " + QString::number(process.pid()));

    emit toLog(command);

    if (!process.waitForStarted(WAIT_FOR_PROCESS)){
        utils::mostrarError("Error al intentar ejecutar " + command);
        this->ui->pushButtonStop->click();
        return;
    }


    this->setStatus(CRACKING);
    //reset values
    this->resetValues();
    //storing last BSSID
    lastBSSID = BSSID;

}

void Aircrack::toThisLog(QString com){
    this->ui->textEdit->append(com);
}

void Aircrack::resetValues(){
    this->ui->spinBoxRead->setValue(0);
    this->ui->spinBoxTested->setValue(0);
}

void Aircrack::setStatus(STATUS s){
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


}
void Aircrack::stop(){

    if (process.state() == QProcess::Running) {
        debug::add("Stopping process aircrack " + QString::number(process.pid()));
        utils::killProcess(process.pid());
        process.kill();
        process.terminate();
        if (!process.waitForFinished(WAIT_FOR_PROCESS))
            utils::mostrarError("Impossible to terminate aircrack process");

    }

    if (this->getStatus() != KEY_OBTAINED)
        this->setStatus(STOPPED);

}

void Aircrack::update(){
    static QString info;
    static int index1;
    do {
        info = process.readLine(); //does not work with readALL
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
            }



            else if (info.contains("No file"))
                this->setStatus(STOPPED);



            else
                toThisLog(info);

        }



    } while (!info.isEmpty());


    //FORCE UPDATE. IF WE DONT FORCE, I DONT KNOW WHY THE ATTACK COULD BE PARALIZED
    if (this->getStatus() == CRACKING && process.state() == QProcess::Running) {
        //debug::add("Forcing update in process aircrack " + QString::number(process.pid()));
        QTimer::singleShot(1000, this, SLOT(update()));
    }

}
