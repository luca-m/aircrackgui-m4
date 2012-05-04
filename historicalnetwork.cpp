#include "historicalnetwork.h"
#include "ui_historicalnetwork.h"

historicalNetwork::historicalNetwork(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::historicalNetwork)
{
    logThread::addLog("Constructor of historical network GUI", logInfo::MAIN);

    ui->setupUi(this);

    connect(this->ui->pushButtonLoad, SIGNAL(clicked()), this, SLOT(load()));
    connect(this->ui->tableWidgetWEP, SIGNAL(itemSelectionChanged()), this, SLOT(updateWindow()));
    connect(this->ui->pushButtonCrack, SIGNAL(clicked()), this, SLOT(crackPushed()));
    connect(this->ui->pushButtonExportKeys, SIGNAL(clicked()), this, SLOT(exportKeys()));
    //hidding formated key
    this->ui->frameKey->hide();
    //hidding crack button
    this->ui->pushButtonCrack->hide();
}

historicalNetwork::~historicalNetwork()
{
    logThread::addLog("Destructor of historical network GUI", logInfo::MAIN);
    delete ui;
}


int historicalNetwork::getStoredIvs(QString BSSID){
    for (int i=0; i<this->ui->tableWidgetWEP->rowCount(); ++i)
        if (this->ui->tableWidgetWEP->item(i, 0)->text() == BSSID)
            if (this->ui->tableWidgetWEP->item(i, 2))
                return this->ui->tableWidgetWEP->item(i, 2)->text().toInt();
    return 0;
}

void historicalNetwork::exportKeys(){
    int nKeys = 0;
    for (int i=0; i<this->ui->tableWidgetWEP->rowCount(); ++i)
        if (!this->ui->tableWidgetWEP->item(i, 4)->text().isEmpty())
            nKeys++;

    if (nKeys == 0){
        utils::mostrarMensaje("No keys to export");
        return;
    }

    const QString file = QFileDialog::getSaveFileName(this);

    if (file.isEmpty())
        return;

    QFile f(file);
    if (!f.open(QIODevice::WriteOnly)){
        utils::mostrarError("Impossible to store keys in " + file);
        return;
    }

    f.write("****************\n");
    f.write("ESSID or BSSID\nKEY\n");
    f.write("****************\n");

    for (int i=0; i<this->ui->tableWidgetWEP->rowCount(); ++i)
        if (!this->ui->tableWidgetWEP->item(i, 4)->text().isEmpty()) {
            if (this->ui->tableWidgetWEP->item(i, 4)->text().contains("Session Saved"))
                continue;
            // if we dont have essid
            if (!this->ui->tableWidgetWEP->item(i, 1))
                f.write(this->ui->tableWidgetWEP->item(i, 0)->text().toLatin1() + "\n");
            else
                f.write(this->ui->tableWidgetWEP->item(i, 1)->text().toLatin1() + "\n");
            f.write(this->myFormatKey(this->ui->tableWidgetWEP->item(i, 4)->text()).toLatin1() + "\n");
            f.write("****************\n");
        }

    f.close();
}

QString historicalNetwork::myFormatKey(const QString keyNoFormat){
    QString key = keyNoFormat;
    //ascii mode
    if (key.contains("ASCII:"))
        return QString(key.split("ASCII:").at(1)).remove(' ').remove(')');


    //key in hex mode
    else if (key.contains("KEY")){
       int index = key.indexOf('[');
       key.remove(0, index+1);
       index = key.indexOf(']');
       key.remove(index, key.size()-1);
       return key.remove(':').remove(' ');
    }

    return keyNoFormat;
}

void historicalNetwork::updateWindow(){

    QList<QTableWidgetItem*> items = this->ui->tableWidgetWEP->selectedItems();
    this->ui->frameKey->hide();
    this->ui->pushButtonCrack->hide();

    //empty, cut.
    if (items.size() == 0)
        return;


    //WPA have less values
    if (items.size()<=3) {
        const QString key = myFormatKey(items.at(1)->text());
        this->ui->pushButtonCrack->hide();
        if (!key.isEmpty() && !key.contains("Saved")) {
            this->ui->lineEditKey->setText(key);
            this->ui->frameKey->show();
        }

        return;
    }

    const QString key = myFormatKey(items.at(4)->text());

    //show formatted key if it is necessary
    //important that these if's are exclusive each other
    if (!key.isEmpty()) {
        this->ui->lineEditKey->setText(key);
        this->ui->frameKey->show();
    }

    else
        this->ui->frameKey->hide();

    //show crack button if it is necessary
    if (items.at(4)->text().contains("Not Found") || items.at(4)->text().isEmpty())
        this->ui->pushButtonCrack->show();
    else
        this->ui->pushButtonCrack->hide();


}

void historicalNetwork::crackPushed(){
    emit crack(this->ui->tableWidgetWEP->selectedItems().at(0)->text());
}


//nice algorithm :)
QStringList historicalNetwork::load() {
    logThread::addLog("Historical Network: Loading table", logInfo::MAIN);

    //variables
    QProcess p;
    QStringList keys;
    QStringList infoList;
    QString aux;
    QFile file;
    QFile faux;
    QString line;
    int index;
    int k;

    //reseting
    while (this->ui->tableWidgetWEP->rowCount() > 0)
        this->ui->tableWidgetWEP->removeRow(this->ui->tableWidgetWEP->rowCount()-1);


    //obtaining all files from CAPTURE_FOLDER
    p.setProcessChannelMode(QProcess::MergedChannels);
    p.start(QString("ls") + " " + CAPTURE_FOLDER);
    p.waitForFinished();

    //storing files
    while (1){
        aux = p.readLine();
        //end files
        if (aux.isEmpty())
            break;
        //taking the files in infoList
        if (aux.contains("kismet.csv"))
            infoList.append(aux.remove("\n"));
        else if (aux.contains(".key"))
            keys.append(aux.remove("\n"));
    }

    //filling fields
    for (int i=0; i<infoList.size(); ++i){
        //open file
        file.setFileName(CAPTURE_FOLDER + infoList.at(i));
        file.open(QIODevice::ReadOnly);
        //reading trash
        file.readLine();
        //reading info
        line = file.readLine();
        if (line.isEmpty()) { // i do not know why could happen this, but we have to avoid it.
            file.close();
            continue;
        }
        //taking BSSID
        aux = ((QString)infoList.at(i)).split('-').at(0);
        //is the BSSID already inserted?
        for (index=0; index<this->ui->tableWidgetWEP->rowCount(); ++index)
            //yes, it is.
            if (this->ui->tableWidgetWEP->item(index,0)->text() == aux) {
                    //updating IVS
                if (line.count(';')>=15)
                    this->ui->tableWidgetWEP->setItem(index, 2, utils::toItem(this->ui->tableWidgetWEP->item(index, 2)->text().toInt() +
                                                                              line.split(';').at(16).toInt()));

                    break; //storing the index
            }

        //no inserted yet
        if (index == ui->tableWidgetWEP->rowCount() && line.split(';').size() >= 12) {
            this->ui->tableWidgetWEP->insertRow(index);
            this->ui->tableWidgetWEP->setItem(index,5,utils::toItem("WEP"));
            //bssid
            this->ui->tableWidgetWEP->setItem(index,0,utils::toItem(line.split(';').at(3)));
            //essid
            this->ui->tableWidgetWEP->setItem(index,1,utils::toItem(line.split(';').at(2)));
            //ivs
            this->ui->tableWidgetWEP->setItem(index,2,utils::toItem(line.split(';').at(16)));
            //channel
            this->ui->tableWidgetWEP->setItem(index,3,utils::toItem(line.split(';').at(5)));
            //key
            for (k=0; k<keys.size(); ++k)
                //is there key?
                if ( ((QString)keys.at(k)).contains(this->ui->tableWidgetWEP->item(index,0)->text())) {
                    faux.setFileName(CAPTURE_FOLDER + keys.at(k));
                    faux.open(QIODevice::ReadOnly);
                    this->ui->tableWidgetWEP->setItem(index, 4, utils::toItem((QString(faux.readLine()).remove("\n"))));
                    faux.close();
                    break; //storing k index
                }
            //nop, there is not.
            if (k==keys.size())
                 this->ui->tableWidgetWEP->setItem(index, 4, utils::toItem(""));


        }

        file.close();
    }

    //obtaining all files from REAVER_FOLDER
    p.setProcessChannelMode(QProcess::MergedChannels);
    p.start((QString)"ls" + " " + REAVER_FOLDER);
    p.waitForFinished();
    QStringList bssidList;
    do {
        bssidList.append(p.readLine());
    } while (!bssidList.at(bssidList.size()-1).isEmpty());

    bssidList.pop_back();


    for (int i=0; i<bssidList.size(); ++i) {
        QString wpcName(bssidList.at(i)+".wpc");
        QString keyName(bssidList.at(i)+".key");
        wpcName.remove(':').prepend(REAVER_FOLDER + bssidList.at(i) + '/').remove('\n');
        keyName.prepend(REAVER_FOLDER + bssidList.at(i) + '/').remove('\n');
        if (QFile::exists(wpcName)) {
            this->ui->tableWidgetWEP->insertRow(ui->tableWidgetWEP->rowCount());
            this->ui->tableWidgetWEP->setItem(ui->tableWidgetWEP->rowCount()-1, 5, utils::toItem("WPA"));
            this->ui->tableWidgetWEP->setItem(ui->tableWidgetWEP->rowCount()-1, 0, utils::toItem(wpcName.split('/').at(1)));
            this->ui->tableWidgetWEP->setItem(ui->tableWidgetWEP->rowCount()-1, 4, utils::toItem("Session Saved"));
            //removing duplicates
            for (int j=0; j<this->ui->tableWidgetWEP->rowCount()-1; ++j)
                if (this->ui->tableWidgetWEP->item(j,0)->text() == wpcName.split('/').at(1))
                    this->ui->tableWidgetWEP->removeRow(j);
        }

        // If there are key
        if (QFile::exists(keyName)){
            QFile keyF(keyName);
            keyF.open(QIODevice::ReadOnly);
            this->ui->tableWidgetWEP->setItem(ui->tableWidgetWEP->rowCount()-1, 4, utils::toItem(QString(keyF.readAll()).remove("\n")));
            keyF.close();
        }
    }

    //returning only list of BSSID
    QStringList auxL;
    QString auxx;
    for (int i=0; i<infoList.size();++i) {
        auxx = infoList.at(i);
        auxx.remove(auxx.indexOf('-'), auxx.size()-auxx.indexOf('-'));
        auxL.append(auxx);
    }

    auxL.removeDuplicates();

    return auxL;
}
