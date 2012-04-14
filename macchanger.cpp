#include "macchanger.h"
#include "ui_macchanger.h"

Macchanger::Macchanger(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Macchanger)
{
    debug::add("Constructor of macChanger object " + QString::number(this->winId()));

    ui->setupUi(this);
    connect(this->ui->pushButtonFake, SIGNAL(clicked()), this, SLOT(fakeMac()));

}

Macchanger::~Macchanger()
{
    debug::add("Destructor of macChanger object " + QString::number(this->winId()));

    delete ui;
}

void Macchanger::loadMac(){
    this->ui->lineEditCurrentMac->setText(utils::getMacAdress(GLOBALS::INTERFACE));
}

void Macchanger::fakeMac(){
    const QString fmac = this->ui->lineEditFakedMac->text();
    if (!utils::validMAC(fmac)){
        this->ui->textEdit->append(utils::htmlRojo("Invalid Mac"));
        return;
    }

    QProcess p;
    p.setProcessChannelMode(QProcess::MergedChannels);
    p.start(MACCHANGER_COM);
    debug::add("Starting Macchanger process " + QString::number(p.pid()));

    p.waitForFinished(WAIT_FOR_PROCESS);
    if (!p.readLine().contains("GNU MAC Changer")) {
        this->ui->textEdit->append(utils::htmlRojo("macchanger not installed"));
        return;
    }

    //disabling iface
    QProcess paux;
    const QString disableIface = "ifconfig " + GLOBALS::INTERFACE + " down";
    paux.execute(disableIface);
    this->ui->textEdit->append(disableIface);
    paux.waitForFinished(WAIT_FOR_PROCESS);

    p.start(MACCHANGER_COM + " -m " + fmac + " " + GLOBALS::INTERFACE);
    p.waitForFinished(WAIT_FOR_PROCESS);

    //enabling iface
    const QString enableIface = "ifconfig " + GLOBALS::INTERFACE + " up";
    paux.execute(enableIface);
    this->ui->textEdit->append(enableIface);
    paux.waitForFinished(WAIT_FOR_PROCESS);



    QString aux;
    aux = p.readLine();

    if (aux.contains("ERROR"))
        this->ui->textEdit->append(utils::htmlRojo(aux));
    else
        this->ui->textEdit->append(utils::htmlVerde(p.readLine()));

    //reloading
    this->loadMac();

}
