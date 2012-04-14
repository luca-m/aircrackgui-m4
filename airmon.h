#ifndef AIRMON_H
#define AIRMON_H

#include <QWidget>
#include "qprocess.h"
#include "utils.h"
#include "QTextStream"
#include "QTimer"
#include "debug.h"


namespace Ui {
    class airmon;
}

class airmon : public QWidget
{
    Q_OBJECT

public:
    explicit airmon(QWidget *parent = 0);
    ~airmon();


private:
    Ui::airmon *ui;
    //get interface selected in the combobox
    QString getInterfaceSelected();
    //** this is required because we do not acces to the textedit meanwhile modeMonitorOn/off is running
    //this is because of waits of the process inside the functions
private slots:
    void modeMonitorOn();
    void modeMonitorOff();
    void toThisLog(QString com);

public slots:
    //load interfaces in the combobox
    bool loadInterfaces();
    //main slots to enable/disable mode monitor
    void enableMonitor();
    void disableMonitor();



signals:
    void toLog(QString com);
    void monitorEnabled(QString interface);

};

#endif // AIRMON_H
