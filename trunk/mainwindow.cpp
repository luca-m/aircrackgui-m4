#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "qiodevice.h"
#include "qprocess.h"
#include "GLOBALS.h"
#include "QDebug"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    logThread::addLog("Constructor of MAINWINDOW GUI", logInfo::MAIN);
    ui->setupUi(this);
    this->init();

}

void MainWindow::init(){
    logThread::addLog("MainWindow: INITIALIZING", logInfo::MAIN);

    QSplashScreen splash(QPixmap(":/icons/icons/main.png"));
    splash.show();


    //initializing
    splash.showMessage(VERSION + "\n *Initializing...", Qt::AlignBottom, Qt::white);

    this->addLog(utils::htmlVerde("Initializing..."));
    this->addLog(utils::htmlNegrita("Creating objects..."));
    //creating objects
    splash.showMessage(VERSION + "\n *Creating objects...", Qt::AlignBottom, Qt::white);

    logThread::addLog("MainWindow: Creating ALL BASIC WIDGETS", logInfo::MAIN);

    airodump = new Airodump();
    //widget airmon
    airm = new airmon();
    //widget aireplay death
    aireDeauth = new aireplayDeauth();
    //widget aireplay auth
    aireAuth = new aireplayAuth();
    //widget aireplay broadcast
    aireBroad = new aireplayBroadcast();
    //
    macchanger = new Macchanger();
    //
    aircrack = new Aircrack();
    //
    options = new Options();
    //
    aireARP = new aireplayArpReplay();
    //
    hist = new historicalNetwork();
    //
    //update = new updater();
    //
    frag = new aireplayFragmentation();
    //
    chop = new aireplayChopChop();
    reav = new reaver();
    updat = new updater();
    //pointer from airodump to historicalnetwork
    airodump->setHistoricalNetwork(hist);

    this->addLog(utils::htmlVerde("Done"));

    this->addLog(utils::htmlNegrita("Configuring windows..."));
    this->setWindowTitle(PROGRAM_NAME);
    //inserting widgets in tab
    this->ui->tabWidget->addTab(airodump, "Airodump");
    this->ui->tabWidget->addTab(aireAuth, "Fake Auth");
    this->ui->tabWidget->addTab(aireDeauth, "Fake Deauth");
    this->ui->tabWidget->addTab(aireBroad, "Broadcast");
    this->ui->tabWidget->addTab(aireARP, "ARP Reinjection");
    this->ui->tabWidget->addTab(frag, "Fragmentation");
    this->ui->tabWidget->addTab(chop, "ChopChop");
    this->ui->tabWidget->addTab(aircrack, "Aircrack");
    this->ui->tabWidget->addTab(reav, "Reaver (WPA-WPS)");
    this->ui->tabWidget->addTab(hist, "Historical Network");
    //centring widgets...
    utils::centrarPantalla(this, utils::ARRIBA);
    utils::centrarPantalla(airm, utils::ARRIBA);
    utils::centrarPantalla(this->macchanger, utils::ARRIBA);
    utils::centrarPantalla(this->updat, utils::ARRIBA);
    this->addLog(utils::htmlVerde("Done"));

    this->addLog(utils::htmlNegrita("Connecting signals and slots between objects..."));
    splash.showMessage(VERSION + "\n *Connecting signals and slots...", Qt::AlignBottom, Qt::white);

    //connecting
    logThread::addLog("MainWindow: Connecting all objects", logInfo::MAIN);

    //options
    connect(this->ui->actionGeneral_options, SIGNAL(triggered()), this->options, SLOT(show()));
    //connect to statusBar
    connect(this, SIGNAL(toStatusBar(QString)), this->ui->statusBar, SLOT(showMessage(QString)));

    // reloading interfaces automatically
    connect(airm, SIGNAL(listInterfacesChanged()), airodump, SLOT(reloadInterfaces()));

    // change inject rate from airodump
    connect(this->airodump, SIGNAL(injectionRateChanged(int)),
            this->options, SLOT(setinjectionRate(int)));

    //connect to update status of each attack in airodump
    connect(this->aireAuth, SIGNAL(statusChanged(QString)), this->airodump, SLOT(updateAuthStatus(QString)));
    connect(this->aireDeauth, SIGNAL(statusChanged(QString)), this->airodump, SLOT(updateDeauthStatus(QString)));
    connect(this->frag, SIGNAL(statusChanged(QString)), this->airodump, SLOT(updateFragmentationStatus(QString)));
    connect(this->chop, SIGNAL(statusChanged(QString)), this->airodump, SLOT(updateChopChopStatus(QString)));
    connect(this->aireBroad, SIGNAL(statusChanged(QString)), this->airodump, SLOT(updateBroadcastStatus(QString)));
    connect(this->aireARP, SIGNAL(statusChanged(QString)), this->airodump, SLOT(updateARPReplayStatus(QString)));
    connect(this->aireAuth, SIGNAL(statusChanged(QString)), this->airodump, SLOT(updateAuthStatus(QString)));
    connect(this->aircrack, SIGNAL(statusChanged(QString)), this->airodump, SLOT(updateAircrackStatus(QString)));
    connect(this->reav, SIGNAL(statusChanged(QString)), this->airodump, SLOT(updateReaverStatus(QString)));



    //show airmon
    connect(this->ui->actionAirmon_ng, SIGNAL(triggered()), airm, SLOT(show()));
    //when macchanger is open, reload the MAC of interface
    connect(this->ui->actionMAC_Changer, SIGNAL(triggered()), this->macchanger, SLOT(loadMac()));
    //connect attack buttons//
    //airodump send to main window type of attack. main window handles them. Open the correct tab of attack, and start the attack in the tab//
    connect(airodump, SIGNAL(doAttackDeauth(QString,QString)), this, SLOT(launchDeauth(QString,QString)));
    connect(airodump, SIGNAL(doAttackAuth(QString,QString)), this, SLOT(launchAuth(QString,QString)));
    connect(airodump, SIGNAL(doAttackBroadcast(QString,QString)), this, SLOT(launchBroadcast(QString,QString)));
    connect(airodump, SIGNAL(doAircrack(QString)), this, SLOT(launchAircrack(QString)));
    connect(airodump, SIGNAL(doAttackArpReplay(QString,QString)), this, SLOT(launchArpReplay(QString,QString)));
    connect(airodump, SIGNAL(doAttackInteractiveARP(QString,QString,QString)), this, SLOT(launchArpReplay(QString,QString,QString)));
    connect(airodump, SIGNAL(doAttackFragmentation(QString,QString)), this, SLOT(launchFragmentation(QString,QString)));
    connect(airodump, SIGNAL(doAttackChopChop(QString,QString)), this, SLOT(launchChopChop(QString,QString)));
    connect(airodump, SIGNAL(doAttackReaver(QString)), this, SLOT(launchReaver(QString)));
    //historical network could do crack
    connect(this->hist, SIGNAL(crack(QString)), this, SLOT(launchAircrack(QString)));
    //qt help
    connect(this->ui->actionAbout_QT, SIGNAL(triggered()), this, SLOT(aboutQt()));
    //this program
    connect(this->ui->actionAbout_this_Program, SIGNAL(triggered()), this, SLOT(aboutThisProgram()));
    //about aircrack-ng
    connect(this->ui->actionAbout_Aircrack_ng, SIGNAL(triggered()), this, SLOT(aboutAircrack()));
    //macchanger
    connect(this->ui->actionMAC_Changer, SIGNAL(triggered()), this->macchanger, SLOT(show()));
    //update window

    connect(this->ui->actionCheck_Updates, SIGNAL(triggered()), this->updat, SLOT(show()));

    //to stop all processes
    //focus
    connect(airodump, SIGNAL(focus(QWidget*)), this, SLOT(focusTab(QWidget*)));
    //stop all emit by airodump
    connect(airodump, SIGNAL(doStopAllAttacks()), this, SLOT(stopAllAttacks()));

    connect(this->ui->actionOpen_Log, SIGNAL(triggered()), this, SLOT(openLogs()));

    this->addLog(utils::htmlVerde("Done"));

    splash.showMessage(VERSION + "\n *Loading interfaces...", Qt::AlignBottom, Qt::white);


    splash.showMessage(VERSION + "\n *Checking binaries...", Qt::AlignBottom, Qt::white);

    //ckecking binaries
    this->addLog(utils::htmlNegrita("checking binaries..."));

    logThread::addLog("MainWindow: Checking binaries", logInfo::MAIN);


    foreach (const QString &program, programsNeeded) {
        this->addLog(program + " ...");
        if (utils::programInstalled(program))
            this->addLog(utils::htmlVerde("Correct."));
        else {
            logThread::addLog("MainWindow: *ERROR: " + program + " NOT FOUND", logInfo::MAIN);
            if (program.contains("M4"))
                this->addLog(utils::htmlRojo("Not found!. Make sure you have compiled aircrack-m4 version and you are in the right folder!"));
            else
                this->addLog(utils::htmlRojo("Not found!. Make sure you have installed program. (example: sudo apt-get install aircrack-ng (that includes packetforge-ng, and other programs), or sudo apt-get install macchanger)"));
        }
    }

    splash.showMessage(VERSION + "\n *Checking structure...", Qt::AlignBottom, Qt::white);

    //checking structure
    if (!QFile::exists(CAPTURE_FOLDER)) {
        this->addLog(utils::htmlVerde("Creating capture folder, because does not exist"));
        QDir::current().mkdir(CAPTURE_FOLDER);
    }

    if (!QFile::exists(ARP_FOLDER)) {
        this->addLog(utils::htmlVerde("Creating ARP_replays folder, because does not exist"));
        QDir::current().mkdir(ARP_FOLDER);
    }

    if (!QFile::exists(FRAG_FOLDER)) {
        this->addLog(utils::htmlVerde("Creating Frag folder, because does not exist"));
        QDir::current().mkdir(FRAG_FOLDER);
    }

    if (!QFile::exists(CHOP_FOLDER)) {
        this->addLog(utils::htmlVerde("Creating Chop folder, because does not exist"));
        QDir::current().mkdir(CHOP_FOLDER);
    }

    if (!QFile::exists(FORGED_ARPS)) {
        this->addLog(utils::htmlVerde("Creating Forged Arps folder, because does not exist"));
        QDir::current().mkdir(FORGED_ARPS);
    }

    if (!QFile::exists(BROADCAST_FOLDER)) {
        this->addLog(utils::htmlVerde("Creating Broadcast Replays folder, because does not exist"));
        QDir::current().mkdir(BROADCAST_FOLDER);
    }

    if (!QFile::exists(REAVER_FOLDER)) {
        this->addLog(utils::htmlVerde("Creating Reaver folder, because does not exist"));
        QDir::current().mkdir(REAVER_FOLDER);
    }

    splash.showMessage(VERSION + "\n *Loading Historical Network...", Qt::AlignBottom, Qt::white);

    this->addLog(utils::htmlNegrita("Loading Historical Network..."));
    this->addLog(utils::listToString(hist->load()));
    this->addLog(utils::htmlVerde("Done"));

    splash.showMessage(VERSION + "\n *Checking for updates..", Qt::AlignBottom, Qt::white);


    this->addLog(utils::htmlNegrita("Checking for updates..."));
    this->updat->checkUpdates();
    this->addLog(utils::htmlVerde("Done"));

    // checking if there are monitor mode in any interface. Othwerwise open airmon
    if (utils::getInterfaceMonitorMode().isEmpty()) {
        this->addLog(utils::htmlNegrita("No monitor interfaces detected. Enable it with airmon"));
        QTimer::singleShot(1500, this->ui->actionAirmon_ng, SLOT(trigger()));
        QTimer::singleShot(2000, airm, SLOT(loadInterfaces()));
    }


    splash.showMessage(VERSION + "\n *Checking UID...", Qt::AlignBottom, Qt::white);

    //CHECKING IF YOU ARE ROOT!!!!
    if (!utils::checkRoot())
        this->addLog(utils::htmlRojo("It seems you are not root! (Your UID != 0). Probably you could not execute anything. You must to be root to execute the program"));

}


void MainWindow::openLogs()
{
    system(QString("xterm -geometry 150x30 -e \"tail -n 50 -f %1\"&").arg(MAIN_LOG).toLatin1().data());
}

void MainWindow::aboutQt(){
    QApplication::aboutQt();
}

void MainWindow::aboutThisProgram(){
    utils::mostrarMensaje(ABOUT_THIS_PROGRAM, PROGRAM_NAME);
}

void MainWindow::aboutAircrack(){
    utils::mostrarMensaje(utils::getAircrackVersionM4(), "Precompiled Aircrack-ng suite [MODIFIED]");
}

void MainWindow::stopAllAttacks(){
    logThread::addLog("MainWindow: Stopping all attacks", logInfo::MAIN);

    QStringList actives;

    //auth
    if (aireAuth->getStatus() != aireplayAuth::STOPPED) {
        actives.append("Authentication Stopped");
        aireAuth->stop();
    }
    //deauth
    if (aireDeauth->getStatus() != aireplayDeauth::STOPPED){
        actives.append("Deauthentication Stopped");
        aireDeauth->stop();
    }
    //broadcast
    if (aireBroad->getStatus() != aireplayBroadcast::STOPPED){
        actives.append("Broadcast Attack Stopped");
        aireBroad->stop();
    }
    //arpreplay
    if (aireARP->getStatus() != aireplayArpReplay::STOPPED){
        actives.append("ARP Replay Attack Stopped");
        aireARP->stop();
    }
    //aircrack
    if (aircrack->getStatus() != Aircrack::STOPPED){
        actives.append("Aircrack Process Stopped");
        aircrack->stop();
    }
    //RECORDAR QUE EN ESTOS 2 ATAKES, UN STOP SIGNIFICA PARAR EL STEP 1 Y EL STEP 2
    //frag
    if (frag->getStatus() != aireplayFragmentation::STOPPED){
        actives.append("Fragmentation Attack Stopped");
        frag->stop();
    }
    //chopchop
    if (chop->getStatus() != aireplayChopChop::STOPPED) {
        actives.append("ChopChop Attack Stopped");
        chop->stop();
    }
    //reaver
    if (reav->getStatus() != reaver::STOPPED) {
        actives.append("Attack Reaver Stopped");
        reav->stop();
    }

    if (!actives.isEmpty())
        utils::mostrarMensaje(utils::listToString(actives));
}

void MainWindow::stopAll(){
    this->addLog("Stopping all processes...");
    logThread::addLog("MainWindow: Stopping all processes", logInfo::MAIN);

    //auth
    aireAuth->stop();
    //deauth
    aireDeauth->stop();
    //broadcast
    aireBroad->stop();
    //arpreplay
    aireARP->stop();
    //airodump
    airodump->stop();
    //aircrack
    aircrack->stop();

    //RECORDAR QUE EN ESTOS 2 ATAKES, UN STOP SIGNIFICA PARAR EL STEP 1 Y EL STEP 2
    //frag
    frag->stop();
    //chopchop
    chop->stop();
    reav->stop();

    this->addLog(utils::htmlVerde("Done"));

}

//unimplemented
void MainWindow::reportBug(){

}

MainWindow::~MainWindow()
{
    logThread::addLog("Destructor of MAINWINDOW GUI", logInfo::MAIN);
    delete ui;
    //deleting objects
    delete this->aireDeauth;
    delete this->aireAuth;
    delete this->airm;
    delete this->airodump;
    delete this->aireBroad;
    delete this->macchanger;
    delete this->aircrack;
    delete this->options;
    delete this->aireARP;
    delete this->hist;
    delete this->frag;
    delete this->chop;
    delete this->reav;

}

void MainWindow::focusTab(QWidget *window){
    if (window != NULL)
        if (this->ui->tabWidget->indexOf(window) != -1)
            this->ui->tabWidget->setCurrentWidget(window);
}

void MainWindow::launchFragmentation(const QString &BSSID, QString MAC){
    logThread::addLog("MainWindow: Launching Fragmentation attack", logInfo::MAIN);

    //visible tab
    this->ui->tabWidget->setCurrentWidget(this->frag);
    //running?
    if (frag->getStatus() == aireplayFragmentation::ATTACKING)
        if (QMessageBox::question(this, "Fragmentation Running",
                                  QString("Attack fragmentation is running.") +
                                  "\nDo you want to launch the attack overwritting previous instance?",
                                  QMessageBox::Yes, QMessageBox::No)  != QMessageBox::Yes) {
            this->ui->tabWidget->setCurrentWidget(airodump);
            return;
        }
    //do attack
    frag->start(BSSID, MAC);
}

void MainWindow::launchChopChop(const QString &BSSID, QString MAC){
    logThread::addLog("MainWindow: Launching ChopChop attack", logInfo::MAIN);

    //visible tab
    this->ui->tabWidget->setCurrentWidget(chop);
    //running?
    if (chop->getStatus() == aireplayChopChop::ATTACKING)
        if (QMessageBox::question(this, "ChopChop Running",
                                  QString("Attack ChopChop is running.") +
                                  "\nDo you want to launch the attack overwritting previous instance?",
                                  QMessageBox::Yes, QMessageBox::No)  != QMessageBox::Yes) {
            this->ui->tabWidget->setCurrentWidget(airodump);
            return;
        }

    //do attack
    chop->start(BSSID, MAC);
}

void MainWindow::launchDeauth(const QString &BSSID, QString MAC){
    logThread::addLog("MainWindow: Launching Deauth attack", logInfo::MAIN);

    //visible tab
    this->ui->tabWidget->setCurrentWidget(aireDeauth);
    //do attack
    aireDeauth->start(BSSID, MAC);
}

void MainWindow::launchAuth(const QString &BSSID, QString MAC){
    logThread::addLog("MainWindow: Launching Auth attack", logInfo::MAIN);

    //visible tab
    this->ui->tabWidget->setCurrentWidget(aireAuth);
    //do attack
    aireAuth->start(BSSID, MAC);
}

void MainWindow::launchBroadcast(const QString &BSSID, QString MAC){
    logThread::addLog("MainWindow: Launching Broadcast attack", logInfo::MAIN);

    //visible tab
    this->ui->tabWidget->setCurrentWidget(aireBroad);

    //running?
    if (aireBroad->getStatus() != aireplayBroadcast::STOPPED)
        if (QMessageBox::question(this, "Broadcast Running",
                                  QString("Attack Broadcast is running.") +
                                  "\nDo you want to launch the attack overwritting previous instance?",
                                  QMessageBox::Yes, QMessageBox::No)  != QMessageBox::Yes){

            this->ui->tabWidget->setCurrentWidget(airodump);
            return;

        }

    //do attack
    aireBroad->start(BSSID, MAC);
}

void MainWindow::launchArpReplay(const QString &BSSID, QString MAC, QString cap){

    logThread::addLog("MainWindow: Launching ARP Replay attack", logInfo::MAIN);

    //visible tab
    this->ui->tabWidget->setCurrentWidget(aireARP);

    //running?
    if (aireARP->getStatus() != aireplayArpReplay::STOPPED)
        if (QMessageBox::question(this, "ARPReplay Running",
                                  QString("Attack ARPReplay is running.") +
                                  "\nDo you want to launch the attack overwritting previous instance?",
                                  QMessageBox::Yes, QMessageBox::No)  != QMessageBox::Yes) {
            this->ui->tabWidget->setCurrentWidget(airodump);
            return;
        }

    //do attack
    aireARP->start(BSSID, MAC, cap);
}

void MainWindow::launchAircrack(const QString &BSSID){
    logThread::addLog("MainWindow: Launching Aircrack attack", logInfo::MAIN);

    this->ui->tabWidget->setCurrentWidget(aircrack);

    //running?
    if (aircrack->getStatus() != Aircrack::STOPPED)
        if (QMessageBox::question(this, "Aircrack Running",
                                  QString("Attack Aircrack is running.") +
                                  "\nDo you want to launch the attack overwritting previous instance?",
                                  QMessageBox::Yes, QMessageBox::No)  != QMessageBox::Yes) {
            this->ui->tabWidget->setCurrentWidget(airodump);
            return;
        }

    //do
    aircrack->start(BSSID);
}


void MainWindow::launchReaver(const QString &BSSID)
{
    logThread::addLog("MainWindow: Launching Reaver attack", logInfo::MAIN);

    this->ui->tabWidget->setCurrentWidget(reav);

    //running?
    if (reav->getStatus() != reaver::STOPPED)
        if (QMessageBox::question(this, "Reaver Running",
                                  QString("Attack Reaver is running.") +
                                  "\nDo you want to launch the attack overwritting previous instance?",
                                  QMessageBox::Yes, QMessageBox::No)  != QMessageBox::Yes) {
            this->ui->tabWidget->setCurrentWidget(airodump);
            return;
        }

    //do
    reav->start(BSSID);

    reav->toReaverLog(utils::htmlNegrita("While reaver is running, DON'T CAPTURE USING AIRODUMP. Otherwise reaver become slow and may fail"));
    // while reaver is running, no capture.
    QTimer::singleShot(1000, airodump, SLOT(stop()));
    QTimer::singleShot(2000, airodump, SLOT(clearTables()));

}


void MainWindow::addLog(QString i){
    this->ui->textEditLog->append(i);
}


