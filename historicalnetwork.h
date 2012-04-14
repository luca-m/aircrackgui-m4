#ifndef HISTORICALNETWORK_H
#define HISTORICALNETWORK_H

#include <QWidget>
#include "qstring.h"
#include "qfile.h"
#include "utils.h"
#include "GLOBALS.h"
#include "QStringList"
#include "qfiledialog.h"
#include "debug.h"

namespace Ui {
    class historicalNetwork;
}

class historicalNetwork : public QWidget
{
    Q_OBJECT

public:
    explicit historicalNetwork(QWidget *parent = 0);
    ~historicalNetwork();

private slots:
    void crackPushed();
    void updateWindow();
    void exportKeys();

public slots:
    QStringList load();
    int getStoredIvs(QString BSSID);
    QString myFormatKey(const QString key);

private:
    Ui::historicalNetwork *ui;

signals:
    //order to crack one BSSID stored
    void crack(QString BSSID);
};

#endif // HISTORICALNETWORK_H
