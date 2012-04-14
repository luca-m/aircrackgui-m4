#include "macchanger.h"
#include "ui_macchanger.h"

Macchanger::Macchanger(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Macchanger)
{
    logThread::addLog("Constructor of Macchanger GUI", logInfo::MAIN);

    ui->setupUi(this);
    connect(this->ui->pushButtonFake, SIGNAL(clicked()), this, SLOT(fakeMac()));
    connect(this->ui->pushButtonLoadInterfaces, SIGNAL(clicked()), this, SLOT(loadInterfaces()));
    connect(this->ui->comboBoxInterfaces, SIGNAL(currentIndexChanged(QString)), this, SLOT(updateCurrentMac(QString)));
    this->ui->comboBoxInterfaces->addItems(utils::getListInterfaces());

}

Macchanger::~Macchanger()
{
    logThread::addLog("Destructor of Macchanger GUI", logInfo::MAIN);
    delete ui;
}

void Macchanger::updateCurrentMac(QString iface)
{
    this->ui->lineEditCurrentMac->setText(utils::getMacAdress(iface));
}

void Macchanger::loadInterfaces()
{
    logThread::addLog("Macchanger: Loading interfaces", logInfo::MAIN);
    this->ui->comboBoxInterfaces->clear();
    this->ui->comboBoxInterfaces->addItems(utils::getListInterfaces());
}

void Macchanger::loadMac(){
    this->ui->lineEditCurrentMac->setText(utils::getMacAdress(this->ui->comboBoxInterfaces->currentText()));
}

void Macchanger::fakeMac(){

    logThread::addLog(QString("Macchanger: Trying to fake MAC %1 with %2")
                      .arg(this->ui->lineEditCurrentMac->text()).arg(this->ui->lineEditFakedMac->text().remove(' ')), logInfo::MAIN);

    if (!utils::programInstalled("macchanger")) {
        this->ui->textEdit->append("Macchanger not installed. You can install it by typing sudo apt-get install macchanger");
        return;
    }

    const QString fmac = this->ui->lineEditFakedMac->text().remove(' ');
    if (!utils::validMAC(fmac)){
        this->ui->textEdit->append(utils::htmlRojo("Invalid Mac"));
        return;
    }

    QProcess p;
    p.setProcessChannelMode(QProcess::MergedChannels);
    p.start("macchanger");

    p.waitForFinished(WAIT_FOR_PROCESS);
    if (!p.readLine().contains("GNU MAC Changer")) {
        this->ui->textEdit->append(utils::htmlRojo("macchanger not installed"));
        return;
    }

    //disabling iface
    logThread::addLog("Macchanger: Disabling interface " + this->ui->comboBoxInterfaces->currentText(), logInfo::MAIN);

    QProcess paux;
    const QString disableIface = "ifconfig " + this->ui->comboBoxInterfaces->currentText() + " down";
    paux.execute(disableIface);
    this->ui->textEdit->append(disableIface);
    paux.waitForFinished(WAIT_FOR_PROCESS);

    p.start("macchanger -m " + fmac + " " + this->ui->comboBoxInterfaces->currentText());
    p.waitForFinished(WAIT_FOR_PROCESS);

    //enabling iface

    logThread::addLog("Macchanger: Enabling interface " + this->ui->comboBoxInterfaces->currentText(), logInfo::MAIN);

    const QString enableIface = "ifconfig " + this->ui->comboBoxInterfaces->currentText() + " up";
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
