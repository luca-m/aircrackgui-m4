#ifndef AIREPLAYAUTH_H
#define AIREPLAYAUTH_H

#include <QWidget>
#include <QTimer>
#include <QFile>
#include <QString>
#include "utils.h"
#include "DEFINES.h"
#include "GLOBALS.h"
#include "attackauth.h"

namespace Ui {
    class aireplayAuth;
}

class aireplayAuth : public QWidget
{
    Q_OBJECT

public:
    explicit aireplayAuth(QWidget *parent = 0);
    ~aireplayAuth();

    enum STATUS {AUTHENTICATED, AUTHENTICATING, STOPPED};

    STATUS getStatus()const {return status;}
    QString getStatusQString();

signals:
    void statusChanged(STATUS s);
    void statusChanged(const QString s);
    void succesfull(QString msg);

public slots:
    bool start(const QString &BSSID, const QString &Mac);
    bool stop();
    void clearLog();

private slots:
    void update(QString line);
    void toThisLog(const QString &com);
    void setStatus(const STATUS &s);

private:
    Ui::aireplayAuth *ui;
    attackAuth *attack;
    STATUS status;
};

#endif // AIREPLAYAUTH_H
