#include "aireplayauth.h"
#include "ui_aireplayauth.h"

aireplayAuth::aireplayAuth(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::aireplayAuth)
{
    debug::add("Constructor of aireplay -1 [auth] object " + QString::number(this->winId()));

    ui->setupUi(this);

    connect(this->ui->pushButtonStart, SIGNAL(clicked()), this, SLOT(start()));
    connect(this->ui->pushButtonStop, SIGNAL(clicked()), this, SLOT(stop()));
    //clear log
    connect(this->ui->pushButtonLog, SIGNAL(clicked()), this, SLOT(clearLog()));
    process.setProcessChannelMode(QProcess::MergedChannels);
    connect(&process, SIGNAL(readyRead()), this, SLOT(update()));
    this->setStatus(STOPPED);
}

aireplayAuth::~aireplayAuth()
{
    debug::add("Destructor of aireplay -1 [auth] object " + QString::number(this->winId()));

    delete ui;
    if (process.state() == QProcess::Running) {
         utils::killProcess(process.pid());
         process.waitForFinished(WAIT_FOR_PROCESS);
     }
}

void aireplayAuth::clearLog(){
    utils::customClearLog(this->ui->textEdit, "Fake Auth Log");
}

void aireplayAuth::setStatus(STATUS s){
    {status = s;}
    switch (s) {
        case STOPPED:
            this->ui->lineEditStatus->setText("STOPPED");
            utils::setBackgroundColor(this->ui->lineEditStatus, utils::RED);
            break;
        case AUTHENTICATING:
            this->ui->lineEditStatus->setText("AUTHENTICATING...");
            utils::setBackgroundColor(this->ui->lineEditStatus, utils::YELLOW);
            break;
        case AUTHENTICATED:
            utils::setBackgroundColor(this->ui->lineEditStatus, utils::GREEN);
            this->ui->lineEditStatus->setText("AUTHENTICATED");
            emit succesfull("AUTHENTICATION SUCCESFULL!");
            break;
    }

}
void aireplayAuth::start(){
    this->start(this->ui->lineEditBSSID->text(), this->ui->spinBoxCount->value(), this->ui->lineEditMAC->text(), false);
}

void aireplayAuth::start(QString BSSID, int count, QString MAC, bool fromOutside){
    //if we launch start from outside, we need to put values in the linesedit and spinbox
    //to clarify to the user, to where is the attack.

    //if we launch many times since airodump attack auth, stop previous process
    if (process.state() == QProcess::Running) {
        this->ui->pushButtonStop->click();
        process.waitForFinished();
    }

    if (fromOutside) {
        this->ui->lineEditBSSID->setText(BSSID);
        this->ui->lineEditMAC->setText(MAC);
        this->ui->spinBoxCount->setValue(count);
        //now, normal start
        //the things are automatically enabled or disabled, because we have defined theses signals in the editor
        //when pulses the start button, the things are disabled
        this->ui->pushButtonStart->click();
        //cut function
        return;
    }


    if (!utils::validMAC(BSSID)){
        utils::mostrarError("Invalid BSSID");
        this->ui->pushButtonStop->click();
        return;
    }

    if (!MAC.isEmpty()) {
        if (!utils::validMAC(BSSID)){
                utils::mostrarError("Invalid MAC");
                this->ui->pushButtonStop->click();
                return;
        }

        else
            MAC = " -h " + MAC; //add flag to mac
    }

    const QString command = AIREPLAY_COM +
                            " -1 " +QString::number(count)  +
                            " -a " + BSSID +
                            MAC +
                            " -o " + QString::number(GLOBALS::AUTH_PACK_BURST) +
                            " -T " + QString::number(GLOBALS::AUTH_RETRIES) +
                            " " + GLOBALS::INTERFACE;

    process.start(command);
    debug::add("Starting aireplay -1 [Auth] process " + QString::number(process.pid()));

    emit toLog(command);

    if (!process.waitForStarted(WAIT_FOR_PROCESS)){
         utils::mostrarError("Error al intentar ejecutar " + command);
         this->ui->pushButtonStop->click();
         return;
     }


    //disabling some options
    this->ui->pushButtonStart->setEnabled(false);
    this->ui->groupBox->setEnabled(false);
    //set status
    this->setStatus(AUTHENTICATING);

}

void aireplayAuth::toThisLog(QString com){
    this->ui->textEdit->append(com);
}

void aireplayAuth::stop(){
    if (process.state() == QProcess::Running) {
        debug::add("Stopping aireplay -1 [Auth] process " + QString::number(process.pid()));

        utils::killProcess(process.pid());
        toThisLog("Process stopped");
    }


    //enabling some options
    this->ui->pushButtonStart->setEnabled(true);
    this->ui->groupBox->setEnabled(true);
    //status
    this->setStatus(STOPPED);
}

void aireplayAuth::update(){
    static QString info;

    do {
        info = process.readLine();
        info.remove("\n");
        info.remove("\r");

        if (!info.isEmpty()) {

            //cheking errors
            if (info.contains("failed") || info.contains("select failed") || info.contains("but the AP uses") || info.contains("No such BSSID available")){
                toThisLog(utils::htmlRojo(info));
                this->ui->pushButtonStop->click();
                return;
            }
            //ckecking succesful
            else if (info.contains("AID: 1")) {
                toThisLog(utils::htmlVerde(info));
                //status
                this->setStatus(AUTHENTICATED);
            }

            //checking not succesfull
            else if (info.contains("unsuccessful")) {
                this->toThisLog(utils::htmlRojo("Attack was unsuccessful. Maybe you are too far from the BSSID."));
                this->setStatus(this->STOPPED);
                this->ui->pushButtonStop->click();
            }

            //Got a deauthentication packet!
            else if (info.contains("Got a deauthentication packet!")) {
                toThisLog(utils::htmlRojo(info));
                this->setStatus(this->AUTHENTICATING);
            }

            //normal information
            else
                toThisLog(info);
        }

    } while (!info.isEmpty());



}
