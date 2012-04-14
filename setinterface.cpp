#include "setinterface.h"
#include "ui_setinterface.h"

setInterface::setInterface(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::setInterface)
{
    logThread::addLog("Constructor of setInterface GUI", logInfo::MAIN);

    ui->setupUi(this);
}

setInterface::~setInterface()
{
    logThread::addLog("Destructor of setInterface GUI", logInfo::MAIN);
    delete ui;
}

QComboBox* setInterface::getComboBox(){
    return this->ui->comboBoxInterface;
}
