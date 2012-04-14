#include "updater.h"
#include "ui_updater.h"

updater::updater(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::updater)
{

    logThread::addLog("Constructor of updater attack GUI", logInfo::MAIN);

    ui->setupUi(this);
    //connect button check updates
    connect(this->ui->pushButtonCheckUpdates, SIGNAL(clicked()), this, SLOT(checkUpdates()));
    //connect to download
    connect(this->ui->pushButtonDownload, SIGNAL(clicked()), this, SLOT(downloadUpdate()));
    //connect to show the window when update is available
    connect(this, SIGNAL(updateAvailable()), this, SLOT(show()));
}


updater::~updater()
{
    logThread::addLog("Destructor of updater attack GUI", logInfo::MAIN);
    delete ui;
}


void updater::downloadUpdate()
{
    QDesktopServices::openUrl(QUrl("http://code.google.com/p/aircrackgui-m4/downloads/list"));
}

void updater::checkUpdates(){
    logThread::addLog("Updater: checking for updates", logInfo::MAIN);

    QFile::remove("index.html");
    this->ui->textEditLog->clear();
    //move cursor to end
    this->ui->textEditLog->moveCursor(QTextCursor::End);

    //getting current version
    this->ui->lineEditCurrentVersion->setText(VERSION);

    //getting latest version
    QProcess p;
    p.start("wget http://code.google.com/p/aircrackgui-m4/");
    if (!p.waitForFinished(5000)) {
        this->ui->textEditLog->append(utils::htmlRojo("Imposible to execute correctly wget to update"));
        return;
    }

    QFile f("index.html");
    if (!f.open(QIODevice::ReadOnly)) {
        this->ui->textEditLog->append(utils::htmlRojo("Imposible to open information obtained to update"));
        return;
    }

    QString pageText = f.readAll();
    f.close();

    int preIndex = pageText.indexOf("[{-]");
    if (preIndex == -1) {
        this->ui->textEditLog->append(utils::htmlRojo("Imposible to open information obtained to update"));
        return;
    }

    pageText.remove(preIndex, 4);

    int postIndex = pageText.indexOf("[-}]");
    if (postIndex == -1) {
        this->ui->textEditLog->append(utils::htmlRojo("Imposible to open information obtained to update"));
        return;
    }

    //set latest version
    this->ui->lineEditLatestVersion->setText(pageText.mid(preIndex, postIndex-preIndex).replace('_', ' '));

    //getting changelog
    preIndex = pageText.indexOf("[*CHANGELOG*]");
    if (preIndex == -1) {
        this->ui->textEditLog->append(utils::htmlRojo("Imposible to open information obtained to update"));
        return;
    }

    pageText.remove(preIndex, 13);

    postIndex = pageText.indexOf("[/*CHANGELOG*]");
    if (postIndex == -1) {
        this->ui->textEditLog->append(utils::htmlRojo("Imposible to open information obtained to update"));
        return;
    }

    const QString changelog = pageText.mid(preIndex, postIndex-preIndex);

    //UPDATED?
    //yes
    if (this->ui->lineEditCurrentVersion->text().remove(' ')
            == this->ui->lineEditLatestVersion->text().remove(' ')) {
        if (this->isVisible())
            this->ui->textEditLog->append(utils::htmlVerde("Program is Up-To-Date"));
    }
    //no
    else {
        emit updateAvailable();
        this->ui->textEditLog->append(utils::htmlVerde("*****************************"));
        this->ui->textEditLog->append(utils::htmlVerde("UPDATE AVAILABLE"));
        this->ui->textEditLog->append(utils::htmlVerde("*****************************"));
        //enabling download button
        this->ui->pushButtonDownload->setEnabled(true);
    }

    //anyway showing changelog

    this->ui->textEditLog->append(utils::htmlVerde("\n-----------------------------"));
    this->ui->textEditLog->append(utils::htmlVerde("Changelog"));
    this->ui->textEditLog->append(utils::htmlVerde("-----------------------------"));
    this->ui->textEditLog->append(changelog);


    //move cursor to start
    this->ui->textEditLog->moveCursor(QTextCursor::Start);

}
