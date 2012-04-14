#include "utils.h"

utils::utils()
{
}

bool utils::programInstalled(QString program){
    logThread::addLog(QString("Utils: Checking if the program %1 is installed").arg(program), logInfo::MAIN);

    QProcess p;
    p.start(program);
    p.waitForFinished(5000);
    const QString out = p.readAll();
    // Problems with tcpdump
    return /*!out.isEmpty() && */ !out.contains("not found") && !out.contains("not installed") && !out.contains("ERROR") &&
            !out.contains("not installed") && !out.contains("no instalado") && !out.contains("No command")
            && !out.contains("not found");
}

QString utils::findIpDest(QString decryptedPacket){

    logThread::addLog(QString("Utils: trying to get IP Destination of decrypted packet=%1").arg(decryptedPacket), logInfo::MAIN);

    if (!utils::programInstalled("tcpdump -s")) {
        utils::mostrarError("tcpdump is not installed, impossible to forge an arp request. Please, install aircrack-ng suite");
        return "";
    }

    //tcpdump -s 0 -n -e -r replay_dec-0627-022301.cap
    QProcess p;
    p.setProcessChannelMode(QProcess::MergedChannels);

    //debug::add("Find IP Dest from decrypted packet " + decryptedPacket);

    p.start("tcpdump -s 0 -n -e -r " + decryptedPacket);
    p.waitForFinished(WAIT_FOR_PROCESS);


    //Correct???
    QString IP;
    QString out = p.readAll();

    //debug::add(out);

    if (out.contains(">")) {
        //removing trash
        int index = out.lastIndexOf(">") - 2;
        while (out.at(index) != ' ') {
            IP.prepend(out.at(index));
            index--;
        }
        return IP;
    }


    //Not a valid paquet to find IP dest
    else {
        logThread::addLog("Utils: not a valid paquet to find IP Dest", logInfo::MAIN);
        return "";
    }


}

QString utils::forgeARP(QString xorFile, QString BSSID, QString MAC, QString IPSource, QString IPDest){

    logThread::addLog(QString("Utils: Forging ARP File, xorFile=%1, BSSID=%2, MAC=%3, IPSource=%4, IpDest=%5")
                      .arg(xorFile).arg(BSSID).arg(MAC).arg(IPSource).arg(IPDest), logInfo::MAIN);

    if (!utils::programInstalled("packetforge-ng")) {
        utils::mostrarError("packetforge-ng is not installed, impossible to forge an arp request. Please, install aircrack-ng suite");
        return "";
    }

    QProcess p;
    p.setProcessChannelMode(QProcess::MergedChannels);

    //checking if the folder BSSID exists. If not, we create it to store there caps
    if (!QFile::exists(FORGED_ARPS + BSSID))
        QDir::current().mkdir(FORGED_ARPS + BSSID);

    /*
    packetforge-ng -0 -a 00:14:6C:7E:40:80 -h 00:0F:B5:88:AC:82 -k 255.255.255.255 -l 255.255.255.255 -y
        fragment-0608-132715.xor -w arp-request-1x.cap
    */
    const QString arpForged = QFileInfo(FORGED_ARPS + BSSID + "/" + "arpRequestFrom-" + MAC + "-.cap").absoluteFilePath();
    const QString com = "packetforge-ng -0 -a " +
            BSSID +
            " -h " + MAC +
            " -k " + IPDest +
            " -l " + IPSource +
            " -y " + xorFile +
            " -w " + arpForged;

    p.start(com);
    p.waitForFinished();

    return arpForged;

}

QString utils::getAircrackVersionM4(){
    QProcess p;
    QString info;
    p.start(AIRCRACK_COM);
    p.waitForStarted();
    p.waitForReadyRead();
    for (int i=0; 5>i; ++i)
        info += p.readLine();

    //p.terminate();
    p.waitForFinished();

    return info;
}

void utils::setBackgroundColor(QLineEdit *L, BACKGROUND color){
    switch (color){
    case utils::RED:
        L->setStyleSheet(QString::fromUtf8("background-color: rgb(255, 0, 0);"));
        break;
    case utils::YELLOW:
        L->setStyleSheet(QString::fromUtf8("background-color: rgb(255, 255, 0);"));
        break;
    case utils::GREEN:
        L->setStyleSheet(QString::fromUtf8("background-color: rgb(0, 170, 0);"));
        break;
    }
}

void utils::setBackgroundColorAutomaticWithText(QLineEdit *L, const QString &status)
{
    if (status == "STOPPED")
        setBackgroundColor(L, RED);
    else if (status == "OPENNING" || status == "CRACKING" || status == "READING" || status == "AUTHENTICATING" ||
             status == "ATTACKING" || status == "DEAUTHENTICATING" || status == "RUNNING")
        setBackgroundColor(L, YELLOW);
    else
        setBackgroundColor(L, GREEN);

    L->setText(status);
}


int utils::dropNumber(QString line){
    QString number;
    for (int i=0; i<line.size(); ++i)
        if (line.at(i) >= '0' && line.at(i) <= '9') {
            for (int j = i; j<line.size() && line.at(j) != ' ';++j)
                number += line.at(j);

            return number.toInt();
        }

    return 0;
}

void utils::customClearLog(QTextEdit *t, QString header){
    t->clear();
    t->setHtml("<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
               "<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
               "p, li { white-space: pre-wrap; }\n"
               "</style></head><body style=\" font-family:'Ubuntu'; font-size:11pt; font-weight:400; font-style:normal;\">\n"
               "<table border=\"0\" style=\"-qt-table-type: root; margin-top:4px; margin-bottom:4px; margin-left:4px; margin-right:4px;\">\n"
               "<tr>\n"
               "<td style=\"border: none;\">\n"
               "<p align=\"center\" style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-weight:600; color:#aa0000;\">" +
               header + "</span></p>\n"
               "<p align=\"center\" style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px; font-weight:600; color:#aa0000;\"></p>\n"
               "<p align=\"center\" style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left"
               ":0px; margin-right:0px; -qt-block-indent:0; text-indent:0px; font-weight:600; color:#aa0000;\"></p></td></tr></table></body></html>");
}


QString utils::dropMac(QString line)
{
    // Find ':'. After that, retrocess 2 positions. Check if the 17 next characters
    // form a MAC ADRESS.

    int index = line.indexOf(':');
    while (!line.isEmpty() && index != -1) {
        line.remove(0, index-2);
        if (utils::validMAC(line.left(17)))
            return line.left(17);
        line.remove(0,2);
        index = line.indexOf(':');
    }

    return "";
}

QString utils::getMacAdress(QString iface){
    logThread::addLog("Utils: get mac adress for interface=" + iface, logInfo::MAIN);

    QProcess p;

    if (!programInstalled("macchanger")) {
        return "";
    }

    p.start("macchanger -s " + iface);
    p.waitForFinished(WAIT_FOR_PROCESS);

    //FIX BUG!!! 0.5.1
    //it seems only with the binary macchanger is not enough in some distributions.
    //problem is fixed if macchanger is installed
    QString aux = p.readLine();

    // new versions of macchanger contains 2 lines;
    // one line with PERMANENT mac, and other with CURRENT mac.
    // we want CURRENT mac
    while (!aux.contains("Current") && !aux.isEmpty())
        aux = p.readLine();

    return dropMac(aux);
}

QString utils::actualTime(){
    char aux[20];
    struct tm *lt; time_t tc = time( NULL );
    lt = localtime( &tc );
    sprintf( aux, "%02d:%02d:%02d  ",lt->tm_hour, lt->tm_min, lt->tm_sec );
    return QString(aux);

}

void utils::centrarPantalla(QWidget *w, POSICION p){
    int posW = (QApplication::desktop()->width()/2) - (w->width()/2);
    int posH = (QApplication::desktop()->height()/2) - (w->height()/2);
    if (p == ARRIBA)
        w->move(posW, posH-150);
    else
        w->move(posW, posH);
}



QString utils::htmlRojo(QString texto){
    return("<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
           "<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
           "p, li { white-space: pre-wrap; }\n"
           "</style></head><body style=\" font-family:'Ubuntu'; font-size:11pt; font-weight:400; font-style:normal;\">\n"
           "<table border=\"0\" style=\"-qt-table-type: root; margin-top:4px; margin-bottom:4px; margin-left:4px; margin-right:4px;\">\n"
           "<tr>\n"
           "<td style=\"border: none;\">\n"
           "<p align=\"center\" style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-weight:600; color:#aa0000;\">"+texto+"</span></p></td></tr></table></body></html>");

}

QString utils::htmlVerde(QString texto){

    return("<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
           "<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
           "p, li { white-space: pre-wrap; }\n"
           "</style></head><body style=\" font-family:'Ubuntu'; font-size:11pt; font-weight:400; font-style:normal;\">\n"
           "<table border=\"0\" style=\"-qt-table-type: root; margin-top:4px; margin-bottom:4px; margin-left:4px; margin-right:4px;\">\n"
           "<tr>\n"
           "<td style=\"border: none;\">\n"
           "<p align=\"center\" style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-weight:600; color:#00aa00;\">"+texto+"</span></p></td></tr></table></body></html>");

}

QString utils::htmlNegrita(QString texto){
    return("<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
           "<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
           "p, li { white-space: pre-wrap; }\n"
           "</style></head><body style=\" font-family:'Ubuntu'; font-size:11pt; font-weight:400; font-style:normal;\">\n"
           "<table border=\"0\" style=\"-qt-table-type: root; margin-top:4px; margin-bottom:4px; margin-left:4px; margin-right:4px;\">\n"
           "<tr>\n"
           "<td style=\"border: none;\">\n"
           "<p align=\"center\" style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-weight:600; color:#000000;\">"+texto+"</span></p></td></tr></table></body></html>");
}

QString utils::getInterfaceMonitorMode(){
    logThread::addLog("Utils: Getting interfaces in monitor mode", logInfo::MAIN);

    QStringList lIfaces = utils::getListInterfaces();
    QProcess p;
    for (int i=0; i<lIfaces.size(); ++i) {
        p.start("iwconfig " + lIfaces.at(i));
        p.waitForFinished(WAIT_FOR_PROCESS);
        if (p.readAll().contains("Mode:Monitor")) {
            logThread::addLog(QString("Utils: Interfaces in monitor mode=%1 detected").arg(lIfaces.at(i)), logInfo::MAIN);
            return lIfaces.at(i);
        }
    }

    logThread::addLog("Utils: No interfaces in monitor mode detected", logInfo::MAIN);

    //none
    return "";
}

QStringList utils::getListInterfaces(){
    logThread::addLog("Utils: Getting list of interfaces", logInfo::MAIN);

    const QString command = "ip link show";
    QStringList lIfaces;
    QString aux;
    QProcess p;
    p.start(command);
    if (!p.waitForStarted(WAIT_FOR_PROCESS)) {
        utils::mostrarError("Error trying to load interfaces");
        return lIfaces;
    }

    //wait for readyread
    if (!p.waitForFinished(WAIT_FOR_PROCESS))
        utils::mostrarError("Error trying to finalize process in loadInterfaces()");

    //adding interfaces
    do {
        aux = p.readLine();
        if (aux.isEmpty())
            break;
        lIfaces.append( (QString)(aux.split(':').at(1)) );
        //next iface
        p.readLine();
    } while (1);

    const QStringList list = lIfaces.replaceInStrings(" ", "");
    return list;
}



QTableWidgetItem* utils::toItem(float n){
    QTableWidgetItem *item = new QTableWidgetItem();
    item->setTextAlignment(Qt::AlignCenter);
    //item->setText(QString::number(n, 'g', 4));
    item->setData(Qt::DisplayRole, QVariant(n));

    return item;

}

QTableWidgetItem* utils::toItem(int n){
    QTableWidgetItem *item = new QTableWidgetItem();
    item->setTextAlignment(Qt::AlignCenter);
    //item->setText(QString::number(n));
    item->setData(Qt::DisplayRole, QVariant(n));

    return item;
}

QTableWidgetItem* utils::toItem(QString a){
    QTableWidgetItem *item = new QTableWidgetItem();
    item->setTextAlignment(Qt::AlignCenter);
    //item->setText(a);
    item->setData(Qt::DisplayRole, QVariant(a));

    return item;
}

QString utils::listToString(QList<QString> list){
    QString aux;
    for (int i=0; i<list.size(); ++i)
        aux += list.at(i) + "\n";

    return aux.remove(aux.size()-1, 1);
}
