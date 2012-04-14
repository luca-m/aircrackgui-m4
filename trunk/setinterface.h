#ifndef SETINTERFACE_H
#define SETINTERFACE_H

#include <QWidget>
#include <QComboBox>
#include "GLOBALS.h"


namespace Ui {
    class setInterface;
}

class setInterface : public QWidget
{
    Q_OBJECT

public:
    explicit setInterface(QWidget *parent = 0);
    ~setInterface();

    QComboBox* getComboBox();

private:
    Ui::setInterface *ui;
};

#endif // SETINTERFACE_H
