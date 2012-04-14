#ifndef UPDATER_H
#define UPDATER_H

#include <QWidget>
#include <QProcess>
#include <QFile>
#include <QTimer>
#include <QTextEdit>
#include <QDesktopServices>
#include <QUrl>
#include "utils.h"



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

private:
    Ui::updater *ui;

signals:
    void updateAvailable();
    void startingUpdate();
    void closeProgram();
};

#endif // UPDATER_H
