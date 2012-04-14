#include "updater.h"
#include "ui_updater.h"

updater::updater(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::updater)
{
    debug::add("Constructor of updater object " + QString::number(this->winId()));

    ui->setupUi(this);
    //connect button check updates
    connect(this->ui->pushButtonCheckUpdates, SIGNAL(clicked()), this, SLOT(checkUpdates()));
    //connect to download
    connect(this->ui->pushButtonDownload, SIGNAL(clicked()), this, SLOT(startDownloadUpdate()));
    //connect to show the window when update is available
    connect(this, SIGNAL(updateAvailable()), this, SLOT(show()));
}


updater::~updater()
{
    debug::add("Destructor of updater object " + QString::number(this->winId()));

    delete ui;
}

void updater::startDownloadUpdate(){
    debug::add("Starting download update");


    //emting signal, to receive by mainwindow and stop all processes
    emit startingUpdate();

    this->ui->textEditLog->append(utils::htmlVerde("*****************************"));
    this->ui->textEditLog->append(("Download in progress, please, WAIT until finish..."));
    this->ui->textEditLog->append(utils::htmlVerde("*****************************"));


    //move cursor to end
    this->ui->textEditLog->moveCursor(QTextCursor::End);

    //launch download
    QTimer::singleShot(500, this, SLOT(downloadUpdate()));
}

void updater::downloadUpdate(){

    if (!utils::programInstalled("unrar")) {
        this->ui->textEditLog->append(utils::htmlRojo("unrar is not installed in the system. Please, install it!"));
        return;
    }

    //DOWNLOADING...
    QProcess p;
    debug::add("Downloading update");
    p.start("wget " + realIP + UPDATED_PACKET + " -O " + UPDATED_PACKET);

    p.waitForFinished(MAX_TIME_DOWNLOADING);
    if (!QFile::exists(UPDATED_PACKET) || QFile(UPDATED_PACKET).size() == 0) {
        this->ui->textEditLog->append(utils::htmlRojo("Error downloading update"));
        return;
    }

    this->ui->textEditLog->append(utils::htmlVerde("Updating files..."));
    system("mv aircrack-GUI back");

    debug::add("Unpacking update");
    p.start("unrar x -y update.rar");

    if (p.waitForFinished(10000)) {
        this->ui->textEditLog->append(utils::htmlVerde("DONE"));
        system("rm back");
        this->close();
        debug::add("UPDATED SUCCESFULLY.");
        utils::mostrarMensaje("Update Succesfully. Closing the program to apply changes...");
        emit closeProgram();

    }

    else {
        this->ui->textEditLog->append(utils::htmlRojo("ERROR EXTRACTING FILES"));
    }





    //disabling download button
    this->ui->pushButtonDownload->setEnabled(false);

    //move cursor to end
    this->ui->textEditLog->moveCursor(QTextCursor::End);

}


void updater::checkUpdates(){

    debug::add("Checking updates");

    //move cursor to end
    this->ui->textEditLog->moveCursor(QTextCursor::End);

    //getting current version
    this->ui->lineEditCurrentVersion->setText(VERSION);

    //using if (this->isVisible()) to avoid shows messages when updating is in background

    //reseting
    this->ui->textEditLog->clear();
    this->ui->textEditLog->moveCursor(QTextCursor::Start);
    realIP.clear();

    //checking program wget
    if (!utils::programInstalled("wget")) {
        if (this->isVisible()) {
            this->ui->textEditLog->append("wget not installed in the system. Install it!");
            this->ui->textEditLog->append("In debian: apt-get install wget");
            this->ui->textEditLog->append("In CentOs-RedHat-etc..: yum install wget");
        }
        return;
    }


    //getting the real IP through the domain

    QProcess p;
    p.start("wget " + IP_UPDATE + " -O index.html");
    debug::add("Getting the real IP through the domain");

    if (!p.waitForFinished(10000) || QFile("index.html").size() == 0) {
        p.kill();
        if (this->isVisible())
            this->ui->textEditLog->append(utils::htmlRojo("Impossible to connect with server updates"));
        return;
    }


    QFile f("index.html");
    f.open(QIODevice::ReadOnly);
    QString aux = f.readAll();
    f.close();
    f.remove();
    /*
     ....
 <frameset rows="*" framespacing="0" border="0" frameborder="NO">
    <frame src="http://XX.XX.XX.XX" name="dot_tk_frame_content" scrolling="auto" noresize>
  </frameset>
    .....
  */
    int index = aux.indexOf("<frame src=");
    if (index == -1) {
        if (this->isVisible())
            this->ui->textEditLog->append(utils::htmlRojo("Error updating [1] (maybe no internet connection?)"));
        return;
    }
    index += strlen("<frame src=") + 1;

    while (aux.at(index) != '"') {
        realIP += aux.at(index);
        index++;
    }


    //getting changelog

    debug::add("Getting Changelog");

    p.start("wget " + realIP + FILE_CHANGELOG + " -O " + FILE_CHANGELOG);
    p.waitForFinished(10000);
    if (!QFile::exists(FILE_CHANGELOG) || QFile(FILE_CHANGELOG).size() == 0) {
        if (this->isVisible())
            this->ui->textEditLog->append(utils::htmlRojo("Impossible to retrieve changelog (does not exists). Time excedeed. Try again."));
            this->ui->textEditLog->append(utils::htmlRojo("Maybe there are problems in server updates. Try again in a few minutes."));

        return;
    }


    f.setFileName(FILE_CHANGELOG);
    f.open(QIODevice::ReadOnly);
    //first line. Latest Version
    this->ui->lineEditLatestVersion->setText(QString(f.readLine()).remove("\n"));


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
    if (this->isVisible()) {
        this->ui->textEditLog->append(utils::htmlVerde("\n-----------------------------"));
        this->ui->textEditLog->append(utils::htmlVerde("Changelog"));
        this->ui->textEditLog->append(utils::htmlVerde("-----------------------------"));
        this->ui->textEditLog->append(this->ui->lineEditLatestVersion->text());
        this->ui->textEditLog->append(f.readAll());
    }

    //move cursor to start
    this->ui->textEditLog->moveCursor(QTextCursor::Start);
    p.kill();
    p.waitForFinished();
    f.close();
}
