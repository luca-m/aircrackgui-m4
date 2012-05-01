#ifndef AIRMON_H
#define AIRMON_H

#include <QWidget>
#include <QProcess>
#include <QTextStream>
#include <QTimer>
#include "utils.h"
#include "GLOBALS.h"

namespace Ui {
    class airmon;
}

class airmon : public QWidget
{
    Q_OBJECT

public:
    explicit airmon(QWidget *parent = 0);
    ~airmon();

signals:
    void toLog(QString com);
    void monitorEnabled(QString interface);
    void listInterfacesChanged();

public slots:
    //load interfaces in the combobox
    bool loadInterfaces();
    //main slots to enable/disable mode monitor
    void enableMonitor();
    void disableMonitor();

private slots:
    void modeMonitorOn();
    void modeMonitorOff();
    void toThisLog(QString com);

private:
    Ui::airmon *ui;
    //get interface selected in the combobox
    QString getInterfaceSelected();


};

#endif // AIRMON_H
