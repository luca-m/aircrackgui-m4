#ifndef REAVER_H
#define REAVER_H

#include <QWidget>
#include "attackreaver.h"

namespace Ui {
class reaver;
}

class reaver : public QWidget
{
    Q_OBJECT

public:
    explicit reaver(QWidget *parent = 0);
    ~reaver();

    enum STATUS {RUNNING, STOPPED, OBTAINED};

    STATUS getStatus() const {return status;}
    QString getStatusQString();

signals:
    void statusChanged(STATUS s);
    void statusChanged(const QString s);

public slots:
    bool start(const QString &bssid);
    bool stop();
    void toReaverLog(const QString &info){toThisLog(info);}

private slots:
    void update(QString line);
    void setStatus(const STATUS &s);
    void toThisLog(const QString &info);
    void clearLog();

    
private:
    attackReaver *attack;
    Ui::reaver *ui;
    STATUS status;
    QString pinKey;
    QString lastBSSID;

};

#endif // REAVER_H
