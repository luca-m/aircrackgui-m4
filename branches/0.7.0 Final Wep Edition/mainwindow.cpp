#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "qiodevice.h"
#include "qprocess.h"
#include "GLOBALS.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    debug::add("Constructor of mainWindow object " + QString::number(this->winId()));

    ui->setupUi(this);
    this->init();

}

void MainWindow::init(){
    QSplashScreen splash(QPixmap(":/icons/icons/main.png"));
    splash.show();


    //initializing
    splash.showMessage(VERSION + "\n *Initializing...", Qt::AlignBottom, Qt::white);
    usleep(75000);

    this->addLog(utils::htmlVerde("Initializing..."));
    this->addLog(utils::htmlNegrita("Creating objects..."));
    //creating objects
    splash.showMessage(VERSION + "\n *Creating objects...", Qt::AlignBottom, Qt::white);
    usleep(75000);

    airodump = new Airodump();
    //widget airmon
    airm = new airmon();
    //widget setInterface
    WsetIface = new setInterface();
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
    update = new updater();
    //
    frag = new fragmentation();
    //
    chop = new chopChop();
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
    this->ui->tabWidget->addTab(hist, "Historical Network");
    //centring widgets...
    utils::centrarPantalla(this, utils::ARRIBA);
    utils::centrarPantalla(airm, utils::ARRIBA);
    utils::centrarPantalla(this->WsetIface, utils::ARRIBA);
    utils::centrarPantalla(this->macchanger, utils::ARRIBA);
    utils::centrarPantalla(this->options, utils::ARRIBA);
    utils::centrarPantalla(this->update, utils::ARRIBA);
    this->addLog(utils::htmlVerde("Done"));

    this->addLog(utils::htmlNegrita("Connecting signals and slots between objects..."));
    splash.showMessage(VERSION + "\n *Connecting signals and slots...", Qt::AlignBottom, Qt::white);
    usleep(75000);

    //connecting
    //options
    connect(this->ui->actionGeneral_options, SIGNAL(triggered()), this->options, SLOT(show()));
    //connect to statusBar
    connect(this, SIGNAL(toStatusBar(QString)), this->ui->statusBar, SLOT(showMessage(QString)));
    //to show set interfaces widget
    connect(this->ui->actionSet_Interface, SIGNAL(triggered()), WsetIface, SLOT(show()));
    //when shows set interface widget, list of interfaces are reloaded
    connect(this->ui->actionSet_Interface, SIGNAL(triggered()), this, SLOT(loadInterfaces()));
    //log emit by airodump
    connect(airodump, SIGNAL(toLog(QString)), this, SLOT(addLog(QString)));
    //log emit by airmon
    connect(airm, SIGNAL(toLog(QString)), this, SLOT(addLog(QString)));
    //log emit by airepayDeauth
    connect(aireDeauth, SIGNAL(toLog(QString)), this, SLOT(addLog(QString)));
    //log emit by auth
    connect(aireAuth, SIGNAL(toLog(QString)), this, SLOT(addLog(QString)));
    //log emit by aireBroad
    connect(aireBroad, SIGNAL(toLog(QString)), this, SLOT(addLog(QString)));
    //log emit by aircrack
    connect(aircrack, SIGNAL(toLog(QString)), this, SLOT(addLog(QString)));
    //log emit by arp replay
    connect(this->aireARP, SIGNAL(toLog(QString)), this, SLOT(addLog(QString)));
    //log emit by frag
    connect(this->frag, SIGNAL(toLog(QString)), this, SLOT(addLog(QString)));
    //log emit by chopChop
    connect(this->chop, SIGNAL(toLog(QString)), this, SLOT(addLog(QString)));
    //succesfull attacks to statusBat
    connect(this->aireAuth, SIGNAL(succesfull(QString)), this->ui->statusBar, SLOT(showMessage(QString)));
    connect(this->aireDeauth, SIGNAL(succesfull(QString)), this->ui->statusBar, SLOT(showMessage(QString)));
    connect(this->aireBroad, SIGNAL(succesfull(QString)), this->ui->statusBar, SLOT(showMessage(QString)));
    connect(this->aireARP, SIGNAL(succesfull(QString)), this->ui->statusBar, SLOT(showMessage(QString)));
    connect(this->frag, SIGNAL(succesfull(QString)), this->ui->statusBar, SLOT(showMessage(QString)));
    connect(this->chop, SIGNAL(succesfull(QString)), this->ui->statusBar, SLOT(showMessage(QString)));
    //inform to status bar
    connect(this->airodump, SIGNAL(statusInform(QString)), this->ui->statusBar, SLOT(showMessage(QString)));




    //show airmon
    connect(this->ui->actionAirmon_ng, SIGNAL(triggered()), airm, SLOT(show()));
    //set the interface to work
    connect(WsetIface->getComboBox(), SIGNAL(currentIndexChanged(QString)), this, SLOT(setIface(QString)));
    //when macchanger is open, reload the MAC of interface
    connect(this->ui->actionMAC_Changer, SIGNAL(triggered()), this->macchanger, SLOT(loadMac()));
    //connect attack buttons//
    //airodump send to main window type of attack. main window handles them. Open the correct tab of attack, and start the attack in the tab//
    connect(airodump, SIGNAL(doAttackDeauth(QString,int,QString)), this, SLOT(launchDeauth(QString,int,QString)));
    connect(airodump, SIGNAL(doAttackAuth(QString,int,QString)), this, SLOT(launchAuth(QString,int,QString)));
    connect(airodump, SIGNAL(doAttackBroadcast(QString,QString)), this, SLOT(launchBroadcast(QString,QString)));
    connect(airodump, SIGNAL(doAircrack(QString)), this, SLOT(launchAircrack(QString)));
    connect(airodump, SIGNAL(doAttackArpReplay(QString,QString)), this, SLOT(launchArpReplay(QString,QString)));
    connect(airodump, SIGNAL(doAttackInteractiveARP(QString,QString,QString)), this, SLOT(launchArpReplay(QString,QString,QString)));
    connect(airodump, SIGNAL(doAttackFragmentation(QString,QString)), this, SLOT(launchFragmentation(QString,QString)));
    connect(airodump, SIGNAL(doAttackChopChop(QString,QString)), this, SLOT(launchChopChop(QString,QString)));
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
    //automatic interface setter when enabling mode monitor
    connect(this->airm, SIGNAL(monitorEnabled(QString)), this, SLOT(setIface(QString)));
    //update window
    connect(this->ui->actionCheck_Updates, SIGNAL(triggered()), this->update, SLOT(show()));
    //to stop all processes
    connect(update, SIGNAL(startingUpdate()), this, SLOT(stopAll()));
    //focus
    connect(airodump, SIGNAL(focus(QWidget*)), this, SLOT(focusTab(QWidget*)));
    //stop all emit by airodump
    connect(airodump, SIGNAL(doStopAllAttacks()), this, SLOT(stopAllAttacks()));
    //close emit by update
    connect(update, SIGNAL(closeProgram()), this, SLOT(close()));


    this->addLog(utils::htmlVerde("Done"));

    splash.showMessage(VERSION + "\n *Loading interfaces...", Qt::AlignBottom, Qt::white);
    usleep(75000);

    this->addLog(utils::htmlNegrita("Loading interfaces..."));
    //loading interfaces
    this->addLog(utils::listToString(utils::getListInterfaces()).replace("\n", " "));
    this->loadInterfaces();
    macchanger->loadMac();
    this->addLog(utils::htmlVerde("Done"));


    this->addLog(utils::htmlNegrita("Finding any interface in monitor mode..."));
    const QString imonitor = utils::getInterfaceMonitorMode();
    if (imonitor.size() != 0) {
        this->addLog(("Found! Setting " + imonitor + " as a Main Interface"));
        this->setIface(imonitor);
    }

    else
        this->addLog("Not found");
    this->addLog(utils::htmlVerde("Done"));


    splash.showMessage(VERSION + "\n *Checking binaries...", Qt::AlignBottom, Qt::white);
    usleep(75000);

    //ckecking binaries
    this->addLog(utils::htmlNegrita("checking binaries..."));
    this->addLog(AIRODUMP_COM + "...");
    if (QFile::exists(AIRODUMP_COM))
        this->addLog(utils::htmlVerde("Correct."));
    else
        this->addLog(utils::htmlRojo("Not found!. Make sure you have compiled aircrack-m4 version and you are in the right folder!"));

    this->addLog(AIRCRACK_COM + "...");
    if (QFile::exists(AIRCRACK_COM))
        this->addLog(utils::htmlVerde("Correct."));
    else
        this->addLog(utils::htmlRojo("Not found!. Make sure you have compiled aircrack-m4 version and you are in the right folder!"));

    this->addLog(AIREPLAY_COM + "...");
    if (QFile::exists(AIREPLAY_COM))
        this->addLog(utils::htmlVerde("Correct."));
    else
        this->addLog(utils::htmlRojo("Not found!. Make sure you have compiled aircrack-m4 version and you are in the right folder!"));

    this->addLog(MACCHANGER_COM + "...");
    if (QFile::exists(MACCHANGER_COM))
        this->addLog(utils::htmlVerde("Correct."));
    else
        this->addLog(utils::htmlRojo("Not found!. Make sure you have binarie of macchanger in the folder!"));



    splash.showMessage(VERSION + "\n *Checking structure...", Qt::AlignBottom, Qt::white);
    usleep(75000);

    //checking structure
    if (!QFile::exists(CAPTURE_FOLDER)) {
        this->addLog(utils::htmlVerde("Creating capture folder, because does not exist"));
        system( ("mkdir " + CAPTURE_FOLDER).toLatin1().data());
    }

    if (!QFile::exists(ARP_FOLDER)) {
        this->addLog(utils::htmlVerde("Creating ARP_replays folder, because does not exist"));
        system( ("mkdir " + ARP_FOLDER).toLatin1().data());
    }

    if (!QFile::exists(FRAG_FOLDER)) {
        this->addLog(utils::htmlVerde("Creating Frag folder, because does not exist"));
        system( ("mkdir " + FRAG_FOLDER).toLatin1().data());
    }

    if (!QFile::exists(CHOP_FOLDER)) {
        this->addLog(utils::htmlVerde("Creating Chop folder, because does not exist"));
        system( ("mkdir " + CHOP_FOLDER).toLatin1().data());
    }

    if (!QFile::exists(FORGED_ARPS)) {
        this->addLog(utils::htmlVerde("Creating Forged Arps folder, because does not exist"));
        system( ("mkdir " + FORGED_ARPS).toLatin1().data());
    }

    if (!QFile::exists(BROADCAST_FOLDER)) {
        this->addLog(utils::htmlVerde("Creating Broadcast Replays folder, because does not exist"));
        system( ("mkdir " + BROADCAST_FOLDER).toLatin1().data());
    }

    splash.showMessage(VERSION + "\n *Loading Historical Network...", Qt::AlignBottom, Qt::white);
    usleep(75000);

    this->addLog(utils::htmlNegrita("Loading Historical Network..."));
    this->addLog(utils::listToString(hist->load()));
    this->addLog(utils::htmlVerde("Done"));

    splash.showMessage(VERSION + "\n *Checking for updates..", Qt::AlignBottom, Qt::white);
    usleep(75000);

    this->addLog(utils::htmlNegrita("Checking for updates..."));
    this->update->checkUpdates();
    this->addLog(utils::htmlVerde("Done"));

    //auto check updates
    connect(&this->autoUpdates, SIGNAL(timeout()), this->update, SLOT(checkUpdates()));
    this->autoUpdates.start(TIME_AUTO_CHECK_UPDATES);


    splash.showMessage(VERSION + "\n *Checking UID...", Qt::AlignBottom, Qt::white);
    usleep(75000);

    //CHECKING IF YOU ARE ROOT!!!!
    if (!utils::checkRoot())
        this->addLog(utils::htmlRojo("It seems you are not root! (Your UID != 0). Probably you could not execute anything. You must to be root to execute the program"));

    //this->addLog(utils::htmlVerde("Initialization finished. if you see any red line, please, try to start the program correctly"));


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
    QStringList actives;

    //auth
    if (aireAuth->getStatus() != aireplayAuth::STOPPED) {
        actives.append("Authentication Stopped");
        aireAuth->stop();
    }
    //deauth
    if (aireDeauth->getStatus() != aireplayDeauth::DEAUTHENTICATING){
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
    if (frag->getStatus() != fragmentation::STOPPED){
        actives.append("Fragmentation Attack Stopped");
        frag->stop();
    }
    //chopchop
    if (chop->getStatus() != chopChop::STOPPED) {
        actives.append("ChopChop Attack Stopped");
        chop->stop();
    }

    if (!actives.isEmpty())
        utils::mostrarMensaje(utils::listToString(actives));
}

void MainWindow::stopAll(){
    this->addLog("Stopping all processes...");

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

    this->addLog(utils::htmlVerde("Done"));

}

//unimplemented
void MainWindow::reportBug(){

}

MainWindow::~MainWindow()
{
    debug::add("Destructor of mainWindow object " + QString::number(this->winId()));

    QSplashScreen splash(QPixmap(":/icons/icons/main.png"));
    splash.show();
    splash.showMessage("by supremo12345", Qt::AlignBottom, Qt::white);
    //FIX: 0.6.8: stopping all before delete objects
    this->stopAll();

    usleep(2000000);


    delete ui;
    //deleting objects
    delete this->aireDeauth;
    delete this->aireAuth;
    delete this->airm;
    delete this->airodump;
    delete this->WsetIface;
    delete this->aireBroad;
    delete this->macchanger;
    delete this->aircrack;
    delete this->options;
    delete this->aireARP;
    delete this->hist;
    delete this->update;
    delete this->frag;
    delete this->chop;

}

void MainWindow::focusTab(QWidget *window){
    if (window != NULL)
        if (this->ui->tabWidget->indexOf(window) != -1)
            this->ui->tabWidget->setCurrentWidget(window);
}

void MainWindow::launchFragmentation(QString BSSID, QString MAC){
    //visible tab
    this->ui->tabWidget->setCurrentWidget(this->frag);
    //running?
    if (frag->getStatus() == fragmentation::ATTACKING)
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

void MainWindow::launchChopChop(QString BSSID, QString MAC){
    //visible tab
    this->ui->tabWidget->setCurrentWidget(chop);
    //running?
    if (chop->getStatus() == chopChop::ATTACKING)
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

void MainWindow::launchDeauth(QString BSSID, int count, QString MAC){
    //visible tab
    this->ui->tabWidget->setCurrentWidget(aireDeauth);
    //do attack
    aireDeauth->start(BSSID, count, MAC, true);
}

void MainWindow::launchAuth(QString BSSID, int count, QString MAC){
    //visible tab
    this->ui->tabWidget->setCurrentWidget(aireAuth);
    //do attack
    aireAuth->start(BSSID, count, MAC, true);
}

void MainWindow::launchBroadcast(QString BSSID, QString MAC){
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

void MainWindow::launchArpReplay(QString BSSID, QString MAC, QString cap){

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

void MainWindow::launchAircrack(QString BSSID){
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

void MainWindow::setIface(QString iface){
    if (!iface.isEmpty() && iface != GLOBALS::INTERFACE){
        addLog("Main Interface: " + iface);
        emit toStatusBar("Main Interface: " + iface);
        GLOBALS::INTERFACE = iface;
    }
}

void MainWindow::addLog(QString i){
    //this->ui->textEditLog->append(utils::actualTime());
    this->ui->textEditLog->append(i);
}

void MainWindow::loadInterfaces(){
    WsetIface->getComboBox()->clear();
    WsetIface->getComboBox()->addItems(utils::getListInterfaces());
}
