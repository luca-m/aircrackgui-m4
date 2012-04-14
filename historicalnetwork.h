#ifndef HISTORICALNETWORK_H
#define HISTORICALNETWORK_H

#include <QWidget>
#include <QString>
#include <QFile>
#include <QStringList>
#include <QFileDialog>
#include "utils.h"
#include "GLOBALS.h"


namespace Ui {
    class historicalNetwork;
}

class historicalNetwork : public QWidget
{
    Q_OBJECT

public:
    explicit historicalNetwork(QWidget *parent = 0);
    ~historicalNetwork();

signals:
    //order to crack one BSSID stored
    void crack(QString BSSID);

public slots:
    QStringList load();
    int getStoredIvs(QString BSSID);
    QString myFormatKey(const QString key);

private slots:
    void crackPushed();
    void updateWindow();
    void exportKeys();

private:
    Ui::historicalNetwork *ui;
};

#endif // HISTORICALNETWORK_H
