#include "aireplaydeauth.h"
#include "ui_aireplaydeauth.h"

aireplayDeauth::aireplayDeauth(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::aireplayDeauth)
{
    debug::add("Constructor of aireplay -0 [deAuth] object " + QString::number(this->winId()));


    ui->setupUi(this);

    connect(this->ui->pushButtonStart, SIGNAL(clicked()), this, SLOT(start()));
    connect(this->ui->pushButtonStop, SIGNAL(clicked()), this, SLOT(stop()));
    connect(this->ui->pushButtonLog, SIGNAL(clicked()), this, SLOT(clearLog()));

    process.setProcessChannelMode(QProcess::MergedChannels);
    connect(&process, SIGNAL(readyRead()), this, SLOT(update()));
    //CONNECT WITH FUNCTION WHEN THE PROCESS END BY HIMSELF
    connect(&process, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(stop()));
    this->setStatus(STOPPED);
}

aireplayDeauth::~aireplayDeauth()
{
    debug::add("Destructor of aireplay -0 [deAuth] object " + QString::number(this->winId()));

    delete ui;
    if (process.state() == QProcess::Running) {
         utils::killProcess(process.pid());
         process.waitForFinished(WAIT_FOR_PROCESS);
     }
}

void aireplayDeauth::clearLog(){
    utils::customClearLog(this->ui->textEdit, "Fake Deauth Log");
}

void aireplayDeauth::setStatus(STATUS s){
    {status = s;}
    switch (s) {
        case STOPPED:
            this->ui->lineEditStatus->setText("STOPPED");
            utils::setBackgroundColor(this->ui->lineEditStatus, utils::RED);
            break;
        case DEAUTHENTICATING:
            this->ui->lineEditStatus->setText("DEAUTHENTICATING...");
            utils::setBackgroundColor(this->ui->lineEditStatus, utils::YELLOW);
            break;
        case DEAUTHENTICATED:
            utils::setBackgroundColor(this->ui->lineEditStatus, utils::GREEN);
            this->ui->lineEditStatus->setText("DEUTHENTICATED");
            break;
    }

}

void aireplayDeauth::toThisLog(QString com){
    this->ui->textEdit->append(com);
}

void aireplayDeauth::start(){

    start(this->ui->lineEditBSSID->text(), this->ui->spinBoxCount->value(), this->ui->lineEditDest->text());

}

void aireplayDeauth::start(QString BSSID, int count, QString MAC, bool fromOutside){

    //if we launch start from outside, we need to put values in the linesedit and spinbox
    //to clarify to the user, to where is the attack.
    if (fromOutside) {
        this->ui->lineEditBSSID->setText(BSSID);
        this->ui->lineEditDest->setText(MAC);
        this->ui->spinBoxCount->setValue(count);
        //now, normal start
        //the things are automatically enabled or disabled, because we have defined theses signals in the editor
        //when pulses the start button, the things are disabled
        this->ui->pushButtonStart->click();
        return; //cut function to not allow execute two times
    }

    stop();

    if (!utils::validMAC(BSSID)){
        utils::mostrarError("Invalid BSSID");
        this->ui->pushButtonStop->click();
        return;
    }

    if (!MAC.isEmpty()) {
        if (!utils::validMAC(BSSID)){
                utils::mostrarError("Invalid MAC DESTINATION");
                this->ui->pushButtonStop->click();
                return;
        }

        else
            MAC = " -c " + MAC; //add flag to mac
    }

    const QString command = AIREPLAY_COM + " -0 " + QString::number(count) + " -a " + BSSID + MAC + " " + GLOBALS::INTERFACE;

    process.start(command);
    debug::add("Starting aireplay -0 [Deauth] process " + QString::number(process.pid()));

    emit toLog(command);

    if (!process.waitForStarted(WAIT_FOR_PROCESS)){ //more time because first time you have to introduce password in kdesudo
         utils::mostrarError("Error al intentar ejecutar " + command);
         this->ui->pushButtonStop->click();
         return;
     }

    //ATTACK TO ALL?
    if (this->ui->lineEditDest->text().isEmpty())
        toThisLog("Sending DeAuth to broadcast -- BSSID: " + this->ui->lineEditBSSID->text() + "...");
    //ATTACK TO ONE MAC?
    else
        toThisLog("Sending 64 directed DeAuth. STMAC: " + this->ui->lineEditDest->text() + "...");



    //disabling some options
    this->ui->pushButtonStart->setEnabled(false);
    this->ui->groupBox->setEnabled(false);
    //status
    this->setStatus(this->DEAUTHENTICATING);

}

void aireplayDeauth::stop(){
    //we enter here when the process end by the user. If enter here, disconnect the finish signal to avoid enter again the function when
    //kill the process
    if (process.state() == QProcess::Running) {
        debug::add("Stopping aireplay -0 [Deauth] process " + QString::number(process.pid()));

        toThisLog("Process stopped");
        disconnect(&process, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(stop()));
        utils::killProcess(process.pid());
        process.waitForFinished();
        connect(&process, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(stop()));
        //status
        this->setStatus(this->STOPPED);
        this->ui->pushButtonStart->setEnabled(true);
        this->ui->groupBox->setEnabled(true);

        return;
    }

    //we enter here when the process ends by himself
    //enabling some options
    //status
    this->setStatus(this->DEAUTHENTICATED);
    this->ui->pushButtonStart->setEnabled(true);
    this->ui->groupBox->setEnabled(true);
    emit succesfull("DEAUTHENTICATION SUCCESFULL!");
}

void aireplayDeauth::update(){
    static QString info;

    do {
    info = process.readLine();
    info.remove("\n");
    //info.remove("\r");

        if (!info.isEmpty()) {
            if (info.contains("failed") || info.contains("select failed") || info.contains("but the AP uses") || info.contains("No such BSSID available")){ //checking errors...
                //const int indexSplit = info.indexOf("is on channel");
                //if (indexSplit != -1)
                  //  info = info.right(info.size()-indexSplit+6);

                toThisLog(utils::htmlRojo(info));
                this->ui->pushButtonStop->click();
                return; //exit recursion, creo k no hace falta
            }

            else
                toThisLog(info);
       }

    } while (!info.isEmpty());


    //cannot do it it seems aireplay have many threads and this function is called many times, and done is printed many times
    /*
    else if (process.state() == QProcess::NotRunning){
        toThisLog(utils::htmlVerde("Done"));
        this->ui->pushButtonStop->click();
    }
    */
}

