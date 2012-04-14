#include "options.h"
#include "ui_options.h"

Options::Options(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Options)
{
    logThread::addLog("Constructor of options GUI", logInfo::MAIN);

    ui->setupUi(this);

    connect(this->ui->pushButtonOk, SIGNAL(clicked()), this, SLOT(applyOptions()));
}

Options::~Options()
{
    logThread::addLog("Destructor of options GUI", logInfo::MAIN);
    delete ui;
}

void Options::applyOptions(){
    GLOBALS::SEND_RATE = this->ui->spinBoxRate->value();
    GLOBALS::AUTH_PACK_BURST = this->ui->spinBoxAuthBurstPackets->value();
    GLOBALS::AUTH_RETRIES = this->ui->spinBoxAuthRetry->value();
    GLOBALS::BRUTEFORCE_FACTOR = this->ui->spinBoxFactor->value();
    GLOBALS::BROADCAST_MAX_PACKET = this->ui->spinBoxBroadcastMaxPack->value();
    GLOBALS::BROADCAST_MIN_PACKET = this->ui->spinBoxBroadcastMinPack->value();
    GLOBALS::CHOP_MAX_PACKET = this->ui->spinBoxChopMaxPack->value();
    GLOBALS::CHOP_MIN_PACKET = this->ui->spinBoxChopMinPack->value();
    GLOBALS::FRAG_MAX_PACKET = this->ui->spinBoxFragMaxPack->value();
    GLOBALS::FRAG_MIN_PACKET = this->ui->spinBoxFragMinPack->value();

    logThread::addLog("Options: Applied", logInfo::MAIN);

}
