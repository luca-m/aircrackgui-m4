#include "setinterface.h"
#include "ui_setinterface.h"

setInterface::setInterface(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::setInterface)
{
    debug::add("Constructor of setInterface object " + QString::number(this->winId()));

    ui->setupUi(this);
}

setInterface::~setInterface()
{
    debug::add("Destructor of setInterface object " + QString::number(this->winId()));

    delete ui;
}

QComboBox* setInterface::getComboBox(){
    return this->ui->comboBoxInterface;
}
