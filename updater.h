#ifndef UPDATER_H
#define UPDATER_H

#include <QWidget>
#include "qprocess.h"
#include "utils.h"
#include "qfile.h"
#include "QTimer"
#include "qtextedit.h"
#include "debug.h"

static const QString IP_UPDATE = "http://aircrack-gui-m4.tk/";
#ifdef __i386__
static const QString FILE_CHANGELOG = "changelog";
static const QString UPDATED_PACKET = "update-v2.rar";
#else
static const QString FILE_CHANGELOG = "changelog64";
static const QString UPDATED_PACKET = "update64-v2.rar";
#endif
static const int MAX_TIME_DOWNLOADING = 600*1000; //600 secs

namespace Ui {
    class updater;
}

class updater : public QWidget
{
    Q_OBJECT

public:
    explicit updater(QWidget *parent = 0);
    ~updater();

public slots:
    void checkUpdates();
private slots:
    void downloadUpdate();
    void startDownloadUpdate();

private:
    Ui::updater *ui;
    QString realIP;

signals:
    void updateAvailable();
    void startingUpdate();
    void closeProgram();
};

#endif // UPDATER_H
