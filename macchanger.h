#ifndef MACCHANGER_H
#define MACCHANGER_H

#include <QWidget>
#include <QString>
#include "utils.h"
#include "GLOBALS.h"
#include "debug.h"

namespace Ui {
    class Macchanger;
}

class Macchanger : public QWidget
{
    Q_OBJECT

public:
    explicit Macchanger(QWidget *parent = 0);
    ~Macchanger();
public slots:
    void loadMac();
private slots:
    void fakeMac();
    void loadInterfaces();
    void updateCurrentMac(QString iface);

private:
    Ui::Macchanger *ui;

signals:
    //void toLog(QString);
};

#endif // MACCHANGER_H
