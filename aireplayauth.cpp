#include "aireplayauth.h"
#include "ui_aireplayauth.h"

aireplayAuth::aireplayAuth(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::aireplayAuth)
{
    logThread::addLog("Constructor of fake auth GUI", logInfo::MAIN);

    attack = new attackAuth();

    ui->setupUi(this);

    this->setStatus(STOPPED);

    connect(attack, SIGNAL(processOutput(QString)), this, SLOT(update(QString)), Qt::QueuedConnection);
    connect(this->ui->pushButtonLog, SIGNAL(clicked()), this, SLOT(clearLog()));
    connect(this->ui->pushButtonStop, SIGNAL(clicked()), this, SLOT(stop()));
}

aireplayAuth::~aireplayAuth()
{
    logThread::addLog("Destructor of fake auth GUI", logInfo::MAIN);
    delete attack;
    delete ui;
}

void aireplayAuth::clearLog(){
    utils::customClearLog(this->ui->textEdit, "Fake Auth Log");
}


QString aireplayAuth::getStatusQString()
{
    switch (status) {
        case STOPPED:
            return "STOPPED";
            break;
        case AUTHENTICATING:
            return "AUTHENTICATING";
            break;
        case AUTHENTICATED:
            return "AUTHENTICATED";
            break;
        default:
            return "UNKNOWN";
            break;
    }
}

void aireplayAuth::setStatus(const STATUS &s){
    if (s != this->getStatus()) {
        switch (s) {
            case STOPPED:
                this->ui->lineEditStatus->setText("STOPPED");
                utils::setBackgroundColor(this->ui->lineEditStatus, utils::RED);
                emit statusChanged(s);
                break;
            case AUTHENTICATING:
                this->ui->lineEditStatus->setText("AUTHENTICATING...");
                utils::setBackgroundColor(this->ui->lineEditStatus, utils::YELLOW);
                emit statusChanged(s);
                break;
            case AUTHENTICATED:
                utils::setBackgroundColor(this->ui->lineEditStatus, utils::GREEN);
                this->ui->lineEditStatus->setText("AUTHENTICATED");
                emit succesfull("AUTHENTICATION SUCCESFULL!");
                emit statusChanged(s);
                break;
        }

        status = s;
        logThread::addLog("Fake Auth: Status changes to " + getStatusQString(), logInfo::MAIN);
        emit statusChanged(getStatusQString());
    }
}

// MAC is used for future uses
bool aireplayAuth::start(const QString &BSSID, const QString &MAC){

    logThread::addLog(QString("Fake Auth: Starting attack with BSSID=%1, MAC=%2").arg(BSSID).arg(MAC), logInfo::MAIN);

    const bool ok = attack->start(BSSID, MAC);
    //set status
    if (ok) {
        this->setStatus(AUTHENTICATING);
        this->ui->labelBSSID->setText("BSSID: " + BSSID);
    }

    return ok;

}

void aireplayAuth::toThisLog(const QString &com){
    this->ui->textEdit->append(com);
}

bool aireplayAuth::stop(){
    logThread::addLog("Fake Auth: Stopping", logInfo::MAIN);

    if (attack->isRunning()) {
        attack->stop();
    }

    this->setStatus(STOPPED);
    return true;
}

void aireplayAuth::update(QString info){
        info.remove("\n");
        info.remove("\r");

        if (!info.isEmpty()) {

            //cheking errors
            if (info.contains("failed") || info.contains("select failed") || info.contains("but the AP uses") || info.contains("No such BSSID available")){
                toThisLog(utils::htmlRojo(info));
                this->stop();
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
                this->stop();
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


}
