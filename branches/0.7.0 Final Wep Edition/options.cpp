#include "options.h"
#include "ui_options.h"

Options::Options(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Options)
{
    debug::add("Constructor of options object " + QString::number(this->winId()));

    ui->setupUi(this);

    connect(this->ui->pushButtonOk, SIGNAL(clicked()), this, SLOT(applyOptions()));
}

Options::~Options()
{
    debug::add("Destructor of options object " + QString::number(this->winId()));

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

    debug::add("Applying new options");

}
