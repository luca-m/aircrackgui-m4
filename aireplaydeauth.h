#ifndef AIREPLAYDEAUTH_H
#define AIREPLAYDEAUTH_H

#include <QWidget>
#include <QTimer>
#include "utils.h"
#include "GLOBALS.h"
#include "DEFINES.h"
#include "attackdeauth.h"

namespace Ui {
    class aireplayDeauth;
}

class aireplayDeauth : public QWidget
{
    Q_OBJECT

public:
    explicit aireplayDeauth(QWidget *parent = 0);
    ~aireplayDeauth();

    enum STATUS {DEAUTHENTICATED, DEAUTHENTICATING, STOPPED};

    STATUS getStatus() const {return status;}
    QString getStatusQString();

signals:
    void toLog(QString command);
    void succesfull(QString msg);
    void statusChanged(const QString &s);

public slots:
    bool start(const QString &BSSID, QString MAC);
    bool stop();
    void clearLog();

private slots:
    void update(QString line);
    void toThisLog(const QString &com);
    void setStatus(const STATUS &s);
    void endAuthentication(int ,QProcess::ExitStatus status);

private:
    Ui::aireplayDeauth *ui;
    attackDeauth *attack;
    STATUS status;
};

#endif // AIREPLAYDEAUTH_H
