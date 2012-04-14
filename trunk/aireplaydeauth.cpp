#include "ui_aireplaydeauth.h"
#include "aireplaydeauth.h"

aireplayDeauth::aireplayDeauth(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::aireplayDeauth)
{
    logThread::addLog("Constructor of fake deauth GUI", logInfo::MAIN);

    qRegisterMetaType<QProcess::ExitStatus>("QProcess::ExitStatus");
    attack = new attackDeauth();

    ui->setupUi(this);

    connect(attack, SIGNAL(processOutput(QString)), this, SLOT(update(QString)), Qt::QueuedConnection);
    connect(this->ui->pushButtonLog, SIGNAL(clicked()), this, SLOT(clearLog()));
    connect(this->ui->pushButtonStop, SIGNAL(clicked()), this, SLOT(stop()));

    //CONNECT WITH FUNCTION WHEN THE PROCESS END BY HIMSELF
    connect(attack->getProcess(), SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(endAuthentication(int,QProcess::ExitStatus)));

    this->setStatus(STOPPED);
}

aireplayDeauth::~aireplayDeauth()
{
    logThread::addLog("Destructor of fake deauth GUI", logInfo::MAIN);
    delete ui;
    delete attack;
}

void aireplayDeauth::clearLog()
{
    utils::customClearLog(this->ui->textEdit, "Fake Deauth Log");
}


QString aireplayDeauth::getStatusQString()
{
    switch (status) {
        case STOPPED:
            return "STOPPED";
            break;
        case DEAUTHENTICATING:
            return "DEAUTHENTICATING";
            break;
        case DEAUTHENTICATED:
            return "DEAUTHENTICATED";
            break;
        default:
            return "UNKNOWN";
            break;
    }

}

void aireplayDeauth::setStatus(const STATUS &s)
{
    {status = s;}
    switch (s) {
        case STOPPED:
            this->ui->lineEditStatus->setText("STOPPED");
            utils::setBackgroundColor(this->ui->lineEditStatus, utils::RED);
            break;
        case DEAUTHENTICATING:
            this->ui->lineEditStatus->setText("DEAUTHENTICATING");
            utils::setBackgroundColor(this->ui->lineEditStatus, utils::YELLOW);
            break;
        case DEAUTHENTICATED:
            utils::setBackgroundColor(this->ui->lineEditStatus, utils::GREEN);
            this->ui->lineEditStatus->setText("DEUTHENTICATED");
            break;
    }

    logThread::addLog("Fake Deauth: Status changes to " + getStatusQString(), logInfo::MAIN);

    emit statusChanged(getStatusQString());

}

void aireplayDeauth::toThisLog(const QString &com)
{
    this->ui->textEdit->append(com);
}


bool aireplayDeauth::start(const QString &BSSID, QString MAC)
{
    logThread::addLog(QString("Fake Deauth: Starting attack with BSSID=%1, MAC=%2").arg(BSSID).arg(MAC)+ getStatusQString(), logInfo::MAIN);

    const bool ok = attack->start(BSSID, MAC);
    //set status
    if (ok) {
        this->setStatus(DEAUTHENTICATING);
        this->ui->labelBSSID->setText("BSSID: " + BSSID);
    }

    return ok;

}

bool aireplayDeauth::stop()
{
    logThread::addLog("Fake Deauth: Stopping", logInfo::MAIN);

    if (attack->isRunning()) {
        attack->stop();
        toThisLog("Process stopped");
    }

    this->setStatus(STOPPED);
    return true;

}

void aireplayDeauth::endAuthentication(int, QProcess::ExitStatus status)
{
    if (status == QProcess::NormalExit) {
        this->setStatus(DEAUTHENTICATED);
        emit succesfull("DEAUTHENTICATION SUCCESFULL!");
    }
}

void aireplayDeauth::update(QString info)
{
    info.remove("\n");
    if (info.count('[') < 3)
        info.remove("\r");

        if (!info.isEmpty()) {
            if (info.contains("failed") || info.contains("select failed") || info.contains("but the AP uses") || info.contains("No such BSSID available")){ //checking errors...
                toThisLog(utils::htmlRojo(info));
                this->stop();
                return; //exit recursion, creo k no hace falta
            }

            else
                toThisLog(info);
       }
}

