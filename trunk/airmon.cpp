#include "airmon.h"
#include "ui_airmon.h"

airmon::airmon(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::airmon)
{
    logThread::addLog("Constructor of airmon GUI", logInfo::MAIN);

    ui->setupUi(this);
    //connecting buttons

    connect(this->ui->pushButtonLoad, SIGNAL(clicked()), this, SLOT(loadInterfaces()));
    connect(this->ui->pushButtonEnable, SIGNAL(clicked()), this, SLOT(enableMonitor()));
    connect(this->ui->pushButtonDisable, SIGNAL(clicked()), this, SLOT(disableMonitor()));
}

airmon::~airmon()
{
    logThread::addLog("Destructor of airmon GUI", logInfo::MAIN);
    delete ui;
}


void airmon::enableMonitor(){
    //disabling button to avoid crashes
    this->ui->pushButtonEnable->setDisabled(true);


    if (!this->ui->comboBox->itemText(0).isEmpty())
        toThisLog("Wait a moment...");
    QTimer::singleShot(100, this, SLOT(modeMonitorOn()));

    //enabling button
    this->ui->pushButtonEnable->setDisabled(false);

}

void airmon::disableMonitor(){
    //disabling button to avoid crashes
    this->ui->pushButtonDisable->setDisabled(true);

    if (!this->ui->comboBox->itemText(0).isEmpty())
        toThisLog("Wait a moment...");
    QTimer::singleShot(100, this, SLOT(modeMonitorOff()));

    //enabling button
    this->ui->pushButtonDisable->setDisabled(false);
}

QString airmon::getInterfaceSelected(){
    return this->ui->comboBox->currentText();
}


void airmon::toThisLog(QString com){
    //this->ui->textEdit->append(utils::actualTime());
    this->ui->textEdit->append(com);
}

void airmon::modeMonitorOn(){
    if (getInterfaceSelected().isEmpty())
        return;

    const QString command = "airmon-ng start " + this->getInterfaceSelected();

    QProcess p;
    p.setProcessChannelMode(QProcess::MergedChannels);
    p.start(command);

    emit toLog(command);

    if (!p.waitForStarted(WAIT_FOR_PROCESS)) {
        utils::mostrarError("Error when launch " + command + " .Maybe airmon is not installed. " +
                            "You can install it by typing sudo apt-get install aircrack-ng");
        logThread::addLog("Airmon: Error when launch " + command  + "Maybe airmon is not installed.", logInfo::MAIN);
        //enabling button
        this->ui->pushButtonEnable->setDisabled(false);
        return;
    }

    //terminating process
    if (!p.waitForFinished(WAIT_FOR_PROCESS)) {
        utils::mostrarError("Error trying to finalize process in ModeMonitorOn()");
        logThread::addLog("Airmon: Error when launch " + command  + "Maybe airmon is not installed.", logInfo::MAIN);
    }

    //we have all information
    //ckecking them
    const QString info = p.readAllStandardOutput();
    //errors
    if (info.contains("root")){
        toThisLog(utils::htmlRojo(info));
    }

    else
        toThisLog(info);

    //MONITOR MODE ENABLED IN ANY INTERFACE?
    if (info.contains("monitor mode enabled")) {
        //taking the name of the new interface created in mode monitor
        int index = info.lastIndexOf("monitor mode enabled on ") + strlen("monitor mode enabled on ");
        QString iaux;
        while (info.at(index) != ')'){
            iaux += info.at(index);
            index++;
        }

        emit monitorEnabled(iaux);
        logThread::addLog("Airmon: monitor mode enabled on " + iaux, logInfo::MAIN);
        this->toThisLog("Setting " + iaux + " as a Main Interface");
    }

    //reloading interfaces
    this->loadInterfaces();

}


void airmon::modeMonitorOff(){
    if (getInterfaceSelected().isEmpty())
        return;

    const QString command = "airmon-ng stop " + this->getInterfaceSelected();

    QProcess p;
    p.setProcessChannelMode(QProcess::MergedChannels);
    p.start(command);

    emit toLog(command);

    if (!p.waitForStarted(WAIT_FOR_PROCESS*3)) {
        utils::mostrarError("Error when launch " + command);
        logThread::addLog("Airmon: Error when launch " + command, logInfo::MAIN);
        //enabling button
        this->ui->pushButtonDisable->setDisabled(false);
        return;
    }



    //terminating process
    if (!p.waitForFinished(WAIT_FOR_PROCESS)) {
        utils::mostrarError("Error trying to finalize process in ModeMonitorOn()");
        logThread::addLog("Airmon: Error trying to finalize process in ModeMonitorOn()", logInfo::MAIN);
    }

    //we have all information
    //ckecking them
    const QString info = p.readAllStandardOutput();
    //errors
    if (info.contains("root")){
        toThisLog(utils::htmlRojo(info));
    }

    else
        toThisLog(info);

    logThread::addLog("Airmon: monitor mode disabled on " + this->getInterfaceSelected(), logInfo::MAIN);

    //reloading interfaces
    this->loadInterfaces();


}

bool airmon::loadInterfaces(){
    logThread::addLog("Airmon: loading interfaces", logInfo::MAIN);

    //disabling button to avoid crashes
    this->ui->pushButtonLoad->setDisabled(true);
    //clearing list of interfaces
    this->ui->comboBox->clear();
    //getting list of interfaces
    const QStringList list = utils::getListInterfaces();
    if (list.isEmpty()) {
        utils::mostrarWarning("No interfaces found");
        this->ui->pushButtonLoad->setDisabled(false);
        return false;
    }

    else
        this->ui->comboBox->addItems(list);

    //showing list of interfaces in log
    for (int i=0; i<list.size(); ++i)
        toThisLog(utils::htmlVerde("Found interface " + list.at(i)));

    toThisLog(utils::htmlNegrita("Number of interfaces found: " + QString::number(list.size())));


    //enabling button
    this->ui->pushButtonLoad->setDisabled(false);

    logThread::addLog("Airmon: interfaces loaded: " + QString::number(list.size()), logInfo::MAIN);

    return true;
}
