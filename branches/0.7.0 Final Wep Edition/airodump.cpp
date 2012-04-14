#include "airodump.h"
#include "ui_airodump.h"

Airodump::Airodump(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Airodump)
{
    debug::add("Constructor of airodump object " + QString::number(this->winId()));

    ui->setupUi(this);
    process = new QProcess(this);
    //initializing
    //this->ui->pushButtonStop->hide();
    //connecting buttons
    connect(this->ui->pushButtonStart, SIGNAL(clicked()), this, SLOT(start()));
    connect(this->ui->pushButtonStop, SIGNAL(clicked()), this, SLOT(stop()));
    //connect button to sort
    connect(this->ui->pushButtonSort, SIGNAL(clicked()), this, SLOT(sort()));
    //connect things to trigger associated stations
    connect(this->ui->tableWidgetAirodumpGeneral, SIGNAL(itemSelectionChanged()), this, SLOT(selectStationsAssociated()));
    //below table, signal when new station appear
    connect(this, SIGNAL(stationAppear(infoConnectionBssid)), this, SLOT(newStationAppear(infoConnectionBssid)));
    connect(this, SIGNAL(BSSIDAppear(infoESSID)), this, SLOT(newBSSIDAppear(infoESSID)));
    //attack deauth
    connect(this->ui->pushButtonDeauth, SIGNAL(clicked()), this, SLOT(attackDeauth()));
    //attack auth
    connect(this->ui->pushButtonAuth, SIGNAL(clicked()), this, SLOT(attackAuth()));
    //attack broadcast
    connect(this->ui->pushButtonBroadcast, SIGNAL(clicked()), this, SLOT(attackBroadcast()));
    //aircrack
    connect(this->ui->pushButtonCrack, SIGNAL(clicked()), this, SLOT(aircrack()));
    //capture
    connect(this->ui->pushButtonCapture, SIGNAL(clicked()), this, SLOT(capture()));
    //arp replay
    connect(this->ui->pushButtonARP, SIGNAL(clicked()), this, SLOT(attackArpReplay()));
    //interactive
    connect(this->ui->pushButtonInteractiveARP, SIGNAL(clicked()), this, SLOT(attackInteractiveARP()));
    //frag
    connect(this->ui->pushButtonFrag, SIGNAL(clicked()), this, SLOT(attackFragmetation()));
    //chop
    connect(this->ui->pushButtonChop, SIGNAL(clicked()), this, SLOT(attackChopChop()));
    //all
    connect(this->ui->pushButtonAllAttacks, SIGNAL(clicked()), this, SLOT(allAttacks()));
    //status
    connect(this, SIGNAL(statusChanged(STATUS)), this, SLOT(enableCorrectOptions(STATUS)));
    //more info
    connect(this->ui->pushButtonMoreInfo, SIGNAL(clicked()), this, SLOT(showMoreInfo()));
    //stop all
    connect(this->ui->pushButtonStopAll, SIGNAL(clicked()), this, SLOT(stopAllAttacks()));



    //disabling things
    this->ui->groupBoxAttackOptions->setEnabled(false);
    this->ui->pushButtonCapture->setEnabled(false);
    this->ui->pushButtonCrack->setEnabled(false);
    this->ui->pushButtonMoreInfo->setEnabled(false);

    process->setProcessChannelMode(QProcess::MergedChannels);


}

Airodump::~Airodump()
{
    debug::add("Destructor of airodump object " + QString::number(this->winId()));

    delete ui;
    if (process->state() == QProcess::Running) {
         utils::killProcess(process->pid());
         process->waitForFinished(WAIT_FOR_PROCESS);
     }
}

void Airodump::test(){
}

void Airodump::start(){
    this->startMonitor(false);
}

void Airodump::capture(){
    this->startMonitor(true);
}

void Airodump::setStatus(STATUS s){
    status = s;
    emit statusChanged(s);
}

void Airodump::stopAllAttacks(){
    emit doStopAllAttacks();
}

void Airodump::allAttacks(){

    if (this->getSelectedInfoESSID() == NULL || this->getSelectedInfoESSID()->getBSSID().isEmpty()){
        utils::mostrarMensaje("Please, select a BSSID in the up table");
        return;
    }

    if (this->getSelectedInfoConnectionBssid() == NULL || !utils::validMAC(this->getSelectedInfoConnectionBssid()->getBSSID()))  //can be (not associated)
        if (QMessageBox::question(this, "Message", "We need any station associated to do the attacks.\n Would you want to do a Fake Auth Attack?",
                                  QMessageBox::Yes, QMessageBox::No) != QMessageBox::Yes)
            return;

    //Broadcast
    attackBroadcast();
    //ARP-REPLAY
    attackArpReplay();
    //Fragmentation
    attackFragmetation();
    //ChopChop
    attackChopChop();

    //focus airodump in the main window
    emit focus(this);
    utils::mostrarMensaje("All attacks launched!");

}

void Airodump::showMoreInfo(){
    QList<QTableWidgetItem*> listE = this->ui->tableWidgetAirodumpGeneral->selectedItems();
    QList<QTableWidgetItem*> listC = this->ui->tableWidgetAirodumpAux->selectedItems();

    if (listE.isEmpty()) //FIX: to avoid crash when start with any selected row
       return;

    const QString BSSID = listE.at(0)->data(Qt::DisplayRole).toString();
    const infoESSID* selected = infoE.get(BSSID);

    if (selected == NULL)
        return;

    //showing info
    /*
    QString mb;
    QString enc;
    QString cipher;
    QString auth;
    */
    QString auth;
    if (selected->getAuth().isEmpty())
        auth = "Not Known Yet";
    else
        auth = selected->getAuth();

    int nStations = 0;

    //fix: if we are capturing, not all rows selected are selected, only one.
    if (this->status == CAPTURING)
        nStations = this->ui->tableWidgetAirodumpAux->rowCount();
    else {
        //Important: There are 7 items per row, so the rows selected is items/7
        for (int i=0; i<listC.size(); i=i+7)
            if (listC.at(i)->text() == BSSID)
                nStations++;
    }

    const QString info =
            "\nBSSID = " + selected->getBSSID() +
            "\nName = " + selected->getName() +
            "\nEncryption = " + selected->getEnc() +
            "\nCipher = "   + selected->getCipher() +
            "\nAuthentication = " + auth +
            "\nNumber Of Stations Associated Now = " + QString::number(nStations);

    utils::mostrarMensaje(info, "More info about BSSID");

}

void Airodump::attackInteractiveARP(){
    infoESSID *infoE = this->getSelectedInfoESSID();
    infoConnectionBssid *infoC = this->getSelectedInfoConnectionBssid();



    if (infoE == NULL || infoE->getBSSID().isEmpty()){
        utils::mostrarMensaje("Please, select a BSSID in the up table");
        return;
    }

    //problem: arp must be from a specific mac to a specific bssid
    QString MAC;
    if (infoC == NULL || infoC->getBSSID().isEmpty()) {
        //I think any arp to that BSSID must works
        //utils::mostrarMensaje("Using our MAC as source MAC to do the interactive attack.\narp.cap must have been produced by our MAC as well" +
        //                      QString(" for do a succesfull attack"));
        MAC = utils::getMacAdress(GLOBALS::INTERFACE);
    }

    else
        MAC = infoC->getStation();

    utils::mostrarMensaje(QString() + "Please, select cap where you have a valid arp capture to\n" +
                          //I think any arp to that BSSID must works
                          //QString("MAC = ") + MAC +
                          "\nBSSID = " + infoE->getBSSID());


    //if we have a folder with caps from that BSSID, looking in it

   QString searchFolder;
   if (QFile::exists(FORGED_ARPS+infoE->getBSSID()))
           searchFolder = FORGED_ARPS+infoE->getBSSID();
   else
       searchFolder = FORGED_ARPS;

   //searchFolder = "./";

   const QString cap = QFileDialog::getOpenFileName(this, "", searchFolder, "*.cap");

   if (cap.isEmpty())
        return;


   //correct
   emit doAttackInteractiveARP(infoE->getBSSID(), MAC, cap);



}

void Airodump::attackFragmetation(){
    infoConnectionBssid *infoC = this->getSelectedInfoConnectionBssid();
    infoESSID *infoE = this->getSelectedInfoESSID();

    if (infoE == NULL || infoE->getBSSID().isEmpty()){
        utils::mostrarMensaje("Please, select a BSSID in the up table");
        return;
    }

    if (infoC == NULL || !utils::validMAC(infoC->getBSSID())) { //can be (not associated)
        if (QMessageBox::question(this, "Message", "We need any station associated to do the attack.\n Would you want to do a Fake Auth Attack?",
                                  QMessageBox::Yes, QMessageBox::No) == QMessageBox::Yes)
            emit doAttackAuth(infoE->getBSSID(), 30, "");

        return;
    }

    emit doAttackFragmentation(infoE->getBSSID(), infoC->getStation());
}


void Airodump::attackChopChop(){
    infoConnectionBssid *infoC = this->getSelectedInfoConnectionBssid();
    infoESSID *infoE = this->getSelectedInfoESSID();

    if (infoE == NULL || infoE->getBSSID().isEmpty()){
        utils::mostrarMensaje("Please, select a BSSID in the up table");
        return;
    }

    if (infoC == NULL || !utils::validMAC(infoC->getBSSID())) { //can be (not associated)
        if (QMessageBox::question(this, "Message", "We need any station associated to do the attack.\n Would you want to do a Fake Auth Attack?",
                                  QMessageBox::Yes, QMessageBox::No)
                                                                                                                            == QMessageBox::Yes)
            emit doAttackAuth(infoE->getBSSID(), 30, "");

        return;
    }

    emit doAttackChopChop(infoE->getBSSID(), infoC->getStation());
}

void Airodump::attackBroadcast(){
    infoConnectionBssid *infoC = this->getSelectedInfoConnectionBssid();
    infoESSID *infoE = this->getSelectedInfoESSID();

    if (infoE == NULL || infoE->getBSSID().isEmpty()){
        utils::mostrarMensaje("Please, select a BSSID in the up table");
        return;
    }

    if (infoC == NULL || !utils::validMAC(infoC->getBSSID())) { //can be (not associated)
        if (QMessageBox::question(this, "Message", "We need any station associated to do the attack.\n Would you want to do a Fake Auth Attack?",
                                  QMessageBox::Yes, QMessageBox::No)
                                                                                                                            == QMessageBox::Yes)
                emit doAttackAuth(infoE->getBSSID(), 30, "");

        return;
    }

    emit doAttackBroadcast(infoE->getBSSID(), infoC->getStation());
}


void Airodump::attackArpReplay(){
    infoConnectionBssid *infoC = this->getSelectedInfoConnectionBssid();
    infoESSID *infoE = this->getSelectedInfoESSID();

    if (infoE == NULL || infoE->getBSSID().isEmpty()){
        utils::mostrarMensaje("Please, select a BSSID in the up table");
        return;
    }

    if (infoC == NULL || !utils::validMAC(infoC->getBSSID())) { //can be (not associated)
        if (QMessageBox::question(this, "Message", "We need any station associated to do the attack.\nWould you want to do a Fake Auth Attack?",
                                  QMessageBox::Yes, QMessageBox::No)
                                                                                                                            == QMessageBox::Yes)
            emit doAttackAuth(infoE->getBSSID(), 30, "");

        return;
    }

    emit doAttackArpReplay(infoE->getBSSID(), infoC->getStation());
}

//depends of the STATE and the tables (if there are selected rows)
void Airodump::enableCorrectOptions(STATUS status){
    if (status == STOPPED) {
        utils::setBackgroundColor(this->ui->lineEditStatus, utils::RED);
        this->ui->lineEditStatus->setText("STOPPED");
        this->ui->groupBoxAttackOptions->setEnabled(false);
        this->ui->pushButtonCapture->setEnabled(false);
        this->ui->pushButtonCrack->setEnabled(false);
        this->ui->pushButtonStart->setEnabled(true);
        this->ui->pushButtonMoreInfo->setEnabled(false);
    }

    else if (status == MONITORING) {
        this->ui->lineEditStatus->setText("MONITORING");
        utils::setBackgroundColor(this->ui->lineEditStatus, utils::YELLOW);
        this->ui->groupBoxAttackOptions->setEnabled(false);// maybe we can set to true
        //row selected?, then active capture and crack
        if (!this->ui->tableWidgetAirodumpGeneral->selectedItems().isEmpty()) {
            this->ui->pushButtonCapture->setEnabled(true);
            this->ui->pushButtonCrack->setEnabled(true);
        }

        else
            this->ui->pushButtonCapture->setEnabled(false);

        //anyway pushbutton start deactivate
        this->ui->pushButtonStart->setEnabled(false);

        //if we are monitoring, we can select multi-rows
        this->ui->tableWidgetAirodumpAux->setSelectionMode(QAbstractItemView::MultiSelection);
        statusInform("Start Monitoring");

    }


    else if (status == CAPTURING) {
        this->ui->lineEditStatus->setText("CAPTURING");
        utils::setBackgroundColor(this->ui->lineEditStatus, utils::GREEN);
        //select the unique row
        if (this->ui->tableWidgetAirodumpGeneral->selectedItems().isEmpty()) //to avoid infinite bucle between selectStationsAssociated and this
            this->ui->tableWidgetAirodumpGeneral->selectRow(0);

        this->ui->pushButtonCrack->setEnabled(true);
        this->ui->groupBoxAttackOptions->setEnabled(true);
        this->ui->pushButtonCapture->setEnabled(false);

        //anyway pushbutton start deactivate
        this->ui->pushButtonStart->setEnabled(false);

        //if we are capturing, only can select one row
        this->ui->tableWidgetAirodumpAux->setSelectionMode(QAbstractItemView::SingleSelection);


        infoESSID *iaux = this->getSelectedInfoESSID();
        if (iaux)
            statusInform("Start Capturing BSSID " + iaux->getBSSID());
        else
            statusInform("Start Capturing");


    }
}

void Airodump::attackDeauth(){
    infoConnectionBssid *infoC = this->getSelectedInfoConnectionBssid();
    infoESSID *infoE = this->getSelectedInfoESSID();

    if (infoE == NULL || infoE->getBSSID().isEmpty()){
        utils::mostrarMensaje("Please, select a BSSID in the up table");
        return;
    }

    //if there are a row of below selected, we already have BSSID + MAC ASSOCIATED
    if (infoC != NULL && !infoC->getBSSID().isEmpty())
        emit doAttackDeauth(infoC->getBSSID(), 10, infoC->getStation());

    //if there are a row of up table selected, we have the BSSID, we can do deauth to all
    else if (!infoE->getBSSID().isEmpty())
        emit doAttackDeauth(infoE->getBSSID(), 10, "");

    else //nothing selected
    utils::mostrarMensaje("Please, select a row of tables to attack. Below table to do Deauth to One Mac Station connected (RECOMMENDED).\n" \
                          "Up table to do a massive deauthentication to all stations connected to that BSSID.");
}

void Airodump::attackAuth(){
    infoESSID *infoE = this->getSelectedInfoESSID();

    if (infoE == NULL || infoE->getBSSID().isEmpty()){
        utils::mostrarMensaje("Please, select a BSSID in the up table");
        return;
    }

    //if there are a row of up table selected
    if (!infoE->getBSSID().isEmpty())
        emit doAttackAuth(infoE->getBSSID(), 30, "");

    else //nothing selected
    utils::mostrarMensaje("Please, select a row of table to attack (BSSID)");
}

void Airodump::aircrack(){
    infoESSID *infoE = this->getSelectedInfoESSID();

    if (infoE == NULL || infoE->getBSSID().isEmpty()){
        utils::mostrarMensaje("Please, select a BSSID in the up table");
        return;
    }

    //if there are a row of up table selected
    if (!infoE->getBSSID().isEmpty())
        emit doAircrack(infoE->getBSSID());

    else //nothing selected
    utils::mostrarMensaje("Please, select a row of table to attack (BSSID)");
}

void Airodump::selectStationsAssociated(){
    if (this->ui->tableWidgetAirodumpGeneral->selectedItems().isEmpty()) //FIX: to avoid crash when start with any selected row
       return;

    //since here, there are any selected row

    //enabling more info
    this->ui->pushButtonMoreInfo->setEnabled(true);

    //reset
    this->ui->tableWidgetAirodumpAux->clearSelection();

    //updating enabled options
    this->enableCorrectOptions(status);

    //taking BSSID to see if he have any station associated
    const QString BSSID = this->ui->tableWidgetAirodumpGeneral->selectedItems().at(0)->data(Qt::DisplayRole).toString();


    for (int i=0; i<this->ui->tableWidgetAirodumpAux->rowCount(); ++i) {
        //if BSSID is in the bottom table, then he has any station associated
        if (this->ui->tableWidgetAirodumpAux->item(i, 0)->data(Qt::DisplayRole).toString() == BSSID) {
            //if the row in the bottom table is not selected
            if (!this->ui->tableWidgetAirodumpAux->item(i, 0)->isSelected()) {
                this->ui->tableWidgetAirodumpAux->selectRow(i);
                //if we are capturing, select ONE row
                if (this->status == CAPTURING)
                    return;
            }
        }
    }



}

void Airodump::newStationAppear(infoConnectionBssid infoC){

    debug::add("STATION " + infoC.getStation() + " appear [Associated with " + infoC.getBSSID() + "]");

    //if we are capturing, we only select the first row in the below table
    //to avoid select always the last row that are being introduced below and avoid little errors
    if (this->status == CAPTURING) {
        QList<QTableWidgetItem*> selectedAUX = this->ui->tableWidgetAirodumpAux->selectedItems();
        if (!selectedAUX.isEmpty())
            return; //break function
    }


    QList<QTableWidgetItem*> selected = this->ui->tableWidgetAirodumpGeneral->selectedItems();
    if (selected.isEmpty()) //FIX: to avoid crash when start with any selected row
       return;

    //if the new station that appear contains a BSSID selected in the bottom table
    if (infoC.getBSSID() == selected.at(0)->data(Qt::DisplayRole).toString())
        this->ui->tableWidgetAirodumpAux->selectRow(this->ui->tableWidgetAirodumpAux->rowCount()-1);

}

void Airodump::newBSSIDAppear(infoESSID infoE){

    debug::add("BSSID " + infoE.getBSSID() + " appear [Name = " + infoE.getName() + "]");

}

void Airodump::startMonitor(bool capture){

    //init
    QString args;

    if (this->ui->checkBoxFilterC->isChecked())
        args += (QString)" -c " += QString::number(this->ui->spinBoxChannel->value());

    if (this->ui->checkBoxFilterE->isChecked())
        args += (QString)" --enc " += this->ui->comboBoxE->currentText();


    QString command;

    if (capture) {
        infoESSID *infoE = this->getSelectedInfoESSID();
        if (infoE->getBSSID().isEmpty()) {
            utils::mostrarError("You must specify a BSSID selecting a row of the up table");
            return;
        }

        /*
        //checking if the folder BSSID exists. If not, we create it to store there caps
        if (!QFile::exists(CAPTURE_FOLDER + infoE->getBSSID()))
            system( QString("mkdir " + CAPTURE_FOLDER + infoE->getBSSID()).toLatin1().data() );
        */



        command = (AIRODUMP_COM + " -c "  + QString::number(infoE->getChannel()) + " --bssid " +
                                 infoE->getBSSID() + " -w " + CAPTURE_FOLDER + infoE->getBSSID()
                                 + " " + GLOBALS::INTERFACE);
    }

    else
        command = (AIRODUMP_COM + " "  + args + " " + GLOBALS::INTERFACE);

   //stopping previus process
   stop();
   //REMOVE ANY PREVIOUS BUFFER
   if (QFile::exists(BUFFER_AIRODUMP_FILENAME))
       if (!fileM4.remove(BUFFER_AIRODUMP_FILENAME))
           utils::mostrarError("Error trying to remove previus .M4 file buffer");


   //START!
   process->start(command);
   debug::add("Starting airodump process " + QString::number(process->pid()));

   emit toLog(command);
   if (!process->waitForStarted(WAIT_FOR_PROCESS))
        utils::mostrarError("Error al intentar ejecutar " + command);
   //checking errors. //Wait for reading enough information

   //work?
   QString info;
   //si el programa sigue corriendo despues de 1 segundos, es que ha funcionado
   if (process->waitForFinished(1000)) {
       info = process->readAllStandardOutput();
       if (!info.isEmpty()) {
           toLog(utils::htmlRojo(info));
           utils::mostrarError(info);
       }
       utils::mostrarError("Are you sure that you have executed Airmon-ng in options\nto put interface into Monitor Mode and select correct\
                           interface?\nSEE LOG");
       //stopping
       this->ui->pushButtonStop->click();
   }

   else if (!this->openFileM4()){
       utils::mostrarError("Error trying to open .M4 file");
       //stopping
       this->ui->pushButtonStop->click();
   }

   else {

        //starting
       //init vectors
       this->clearInfoVectors();
       //clear tables
       this->clearTables();
       update();

       //setStatus is the latest to do (to avoid problems associated a enableCorrectOptions)
       if (capture)
           this->setStatus(CAPTURING);
       else
           this->setStatus(MONITORING);
    }


}


bool Airodump::openFileM4(){
    debug::add("Open file " + BUFFER_AIRODUMP_FILENAME + " as airodump buffer");

    fileM4.setFileName(BUFFER_AIRODUMP_FILENAME);
    return fileM4.open(QIODevice::ReadOnly);
}

void Airodump::stop(){
    //terminating process
    if (process->state() == QProcess::Running) {
        debug::add("Stopping airodump process " + QString::number(process->pid()));

        utils::killProcess(process->pid());
        if (!process->waitForFinished(WAIT_FOR_PROCESS))
            utils::mostrarError("Critical. Error trying to finalize the process airodump");

        statusInform("Airodump Stopped");


    }
    //closing buffer
    this->fileM4.close();
    //de-selecting tables
    this->ui->tableWidgetAirodumpAux->clearSelection();
    this->ui->tableWidgetAirodumpGeneral->clearSelection();
    //status
    this->setStatus(STOPPED);
}

void Airodump::update(){

    //taking BSSIDS
    convertInfo();
    //inserting values
    for (int i=0; !infoE[i].getBSSID().isEmpty(); ++i)
        insertRow(infoE[i], this->ui->tableWidgetAirodumpGeneral);
    for (int i=0; !infoC[i].getBSSID().isEmpty(); ++i)
        insertRow(infoC[i], this->ui->tableWidgetAirodumpAux);

    if (process->state() == QProcess::Running)
        QTimer::singleShot(TIME_UPDATE_AIRODUMP, this, SLOT(update()));
}


void Airodump::sort(){
    //activate sorting
    this->ui->tableWidgetAirodumpGeneral->setSortingEnabled(true);

    //0 = POWER
    //1 = Beacons
    //2 = ivs/s
    //3 = current ivs
    //4 = stored ivs
    this->ui->tableWidgetAirodumpGeneral->sortByColumn(this->ui->comboBoxSort->currentIndex() + 2, Qt::DescendingOrder);


    //disabling sorting
    this->ui->tableWidgetAirodumpGeneral->setSortingEnabled(false);

}

//CRITICAL FUNCTION. Convert info from buffer.
int Airodump::convertInfo(){

    static int nInfoE, nInfoC;
    static QStringList fields;
    //close the buffer to re-read it (because it is updated all time)
    fileM4.close();
    if (!this->fileM4.open(QIODevice::ReadOnly))
        utils::mostrarError("Error trying to re-open .M4 to be read");

    //init index of vectors
    nInfoE = nInfoC = 0;

    //nothing to read?
    if (fileM4.readLine().isEmpty())
        return -1;

    //reading trash
    fileM4.readLine();

    //filling first table
    while (1) {
        fields = QString(fileM4.readLine()).split(',');
        fields.replaceInStrings(" ", "");
        if (fields.size() != 16)
            break;

        this->infoE[nInfoE].setAuth(fields.at(7));
        this->infoE[nInfoE].setBeacons(fields.at(9));
        this->infoE[nInfoE].setBSSID(fields.at(0));
        this->infoE[nInfoE].setChannel(fields.at(3));
        this->infoE[nInfoE].setCipher(fields.at(6));
        this->infoE[nInfoE].setData(fields.at(10));
        this->infoE[nInfoE].setDataS(fields.at(14));
        this->infoE[nInfoE].setEnc(fields.at(5));
        this->infoE[nInfoE].setMb(fields.at(4));
        this->infoE[nInfoE].setName(fields.at(13));
        this->infoE[nInfoE].setPower(fields.at(8));

        nInfoE++;
    }

    //reading trash
    fileM4.readLine();

    //filling second table
    while (1){
        fields = QString(fileM4.readLine()).split(',');
        fields.replaceInStrings(" ", "");
        if (fields.size() != 9)
            break;

        this->infoC[nInfoC].setBSSID(fields.at(5));
        this->infoC[nInfoC].setLost(fields.at(6));
        this->infoC[nInfoC].setPackets(fields.at(4));
        this->infoC[nInfoC].setPower(fields.at(3));
        this->infoC[nInfoC].setProbes(((QString)fields.at(8))); //aki hay caracteres erroneos
        this->infoC[nInfoC].setRate(fields.at(7));
        this->infoC[nInfoC].setStation(fields.at(0));

        nInfoC++;

    }
    return 1;


}

bool Airodump::insertRow(const infoConnectionBssid &infoC, QTableWidget *table){
    //infoC empty?
    if (infoC.getBSSID().isEmpty())
        return false;

    //init
    int index = 0;
    bool newStation = false;

    //is the infoC ALREADY in the table?
    for (index=0; index<table->rowCount(); ++index)
        //SUPER FIX!!! ROW IS NOT THE SAME IF THE STATION IS DIFFERENT!!!!!!!!!!!!1
            if (table->item(index,1)->text() == infoC.getStation()) //yes, it is. We store the index
                break;

    //no, it is not, insert new row.
    if (index == table->rowCount()) {
            QProgressBar *p = new QProgressBar(this);  //is sure that we destroy it?
            table->insertRow(index);
            table->setCellWidget(index, 2, p);
            p->setValue(0);
            newStation = true;
    }

    //insert items of row
    table->setItem(index, 0, utils::toItem(infoC.getBSSID()));
    table->setItem(index, 1, utils::toItem(infoC.getStation()));
    ((QProgressBar *)table->cellWidget(index,2))->setValue(infoC.getPower());
    table->setItem(index, 2, utils::toItem(infoC.getPower())); //we need to insert also this item to can sorting by it
    table->setItem(index, 3, utils::toItem(infoC.getRate()));
    table->setItem(index, 4, utils::toItem(infoC.getLost()));
    table->setItem(index, 5, utils::toItem(infoC.getPackets()));
    table->setItem(index, 6, utils::toItem(infoC.getProbes()));

    if (newStation) //new station is entered
        emit stationAppear(infoC);

    //all correct
    return true;



}

bool Airodump::insertRow(const infoESSID &info, QTableWidget *table){
    //info empty?
    if (info.getBSSID().isEmpty())
        return false;
    //init
    int index = 0;
    bool newBSSID = false;
     //is the info ALREADY in the table?
    for (index=0; index<table->rowCount(); ++index)
            if (table->item(index,0)->text() == info.getBSSID()) //yes, it is. We store the index
                break;

    //no, it is not, insert new row.
    if (index == table->rowCount()) {
            QProgressBar *p = new QProgressBar(this); //is sure that we destroy it?, SI CREO QUE SE DESTRUYE, porque el padre lo destruye cuando muere.
            table->insertRow(index);
            table->setCellWidget(index, 2, p);
            //inserting ONE TIME stored IVS
            table->setItem(index, 6, utils::toItem(histAux->getStoredIvs(info.getBSSID())));
            newBSSID = true;
    }

    //insert items of row
    table->setItem(index, 0, utils::toItem(info.getBSSID()));
    table->setItem(index, 1, utils::toItem(info.getName()));
    ((QProgressBar *)table->cellWidget(index,2))->setValue(info.getPowerConverted());
    table->setItem(index, 2, utils::toItem(info.getPowerConverted())); //we need to insert also this item to can sorting by it
    table->setItem(index, 3, utils::toItem(info.getBeacons()));
    table->setItem(index, 4, utils::toItem(info.getDataS()));
    table->setItem(index, 5, utils::toItem(info.getData()));
    table->setItem(index, 7, utils::toItem(info.getChannel()));

    if (newBSSID)
        emit BSSIDAppear(info);


    return true;
}

infoESSID* Airodump::getSelectedInfoESSID(){
    QList<QTableWidgetItem*> items = this->ui->tableWidgetAirodumpGeneral->selectedItems(); //one row
    if (items.isEmpty()) //no selection?
        return NULL;

    for (int i=0; i < MAX_ESSID && this->infoE[i].getBSSID() != ""; ++i)
        if (items.at(0)->text() == this->infoE[i].getBSSID())
            return &this->infoE[i];

    utils::mostrarError("Maximum of ESSID reached, limit is " + MAX_ESSID);
    //FIX CRITICAL BUG [thx drvalium]. WHEN LIMIT IS REACHED, RETURN NULL
    return NULL;
}



infoConnectionBssid* Airodump::getSelectedInfoConnectionBssid(){
    QList<QTableWidgetItem*> items = this->ui->tableWidgetAirodumpAux->selectedItems(); //one row
    if (items.isEmpty()) //no selection?
        return NULL;

    //FIX SUPER BUG 0.6.8: BSSID COULD BE THE SAME (MANY STATIONS ASSOCIATED TO HIM)
    //BUT WE HAVE TO CHOOSE THE SELECTED ROW
    //BEFORE WE SELECTED RANDOM STATION ASSOCIATED, NOT THE SELECTED ONE
    for (int i=0; i<MAX_BSSID_CON && this->infoC[i].getBSSID() != ""; ++i)
        if (items.at(1)->text() == this->infoC[i].getStation())
            return &this->infoC[i];

    utils::mostrarError("Maximum of ESSID-Stations associated reached, limit is " + MAX_BSSID_CON);

    return NULL;

}

//a implementar
infoConnectionBssid Airodump::getRowInfoConnectionBssid(int row){

}



void Airodump::clearInfoVectors(){
    infoE.clear();
    infoC.clear();
}


void Airodump::clearTables(){

    while (this->ui->tableWidgetAirodumpAux->rowCount() > 0)
        this->ui->tableWidgetAirodumpAux->removeRow(this->ui->tableWidgetAirodumpAux->rowCount()-1);
    while (this->ui->tableWidgetAirodumpGeneral->rowCount() > 0)
        this->ui->tableWidgetAirodumpGeneral->removeRow(this->ui->tableWidgetAirodumpGeneral->rowCount()-1);

}
