#ifndef DEFINES_H
#define DEFINES_H
#include <QString>
#include "QStringList"
static const int _VERSION = 1;
static const int _SUBVERSION = 0;
static const int _MIN_VERSION = 0;
static const int _BUILD = 2;
static const QString _MIN_NAME = " -ULTIMATE EDITION-";
static const QString _STATUS_VERSION = "BETA " + QString::number(_BUILD);
static const QString ABOUT_THIS_PROGRAM =
"GUI for monitor wireless. Front-end of Aircrack-ng suite. Includes\
 others wireless tools like macchanger.\n\
This program has been created to increase performance and accesibility\
to the controlling and scanning wireless networks.\n\n\
Programmer: Manuel Garcia Sanz (supremo12345)\n\
Contact: manuelgs88@gmail.com\n\n\
Project Page: http://code.google.com/p/aircrackgui-m4/ \n\n\
Donators that CONTRIBUTE WITH A SYMBOLIC AMOUNT: \n\
srx (elhacker.net) - 3euro \n\n\n\
Thx to:\n\
http://foro.seguridadwireless.net/\n\
USUARIONUEVO (*special mention*)\n\
pac0\n\
sanson\n\
drvalium\n\
";

static const QString PROJECT_PAGE = "http://code.google.com/p/aircrackgui-m4/";
static const QString REPORT_BUG_PAGE = "http://code.google.com/p/aircrackgui-m4/issues/entry";
static const QString VERSION = QString::number(_VERSION) + "." + QString::number(_SUBVERSION) + "." + QString::number(_MIN_VERSION) + " " + _STATUS_VERSION + " " + _MIN_NAME;
static const QString PROGRAM_NAME = "Aircrack-ng [M4-GUI-Version] " + VERSION;

static const int TIME_AUTO_CHECK_UPDATES = 1800000; //ms (half hour)
static const int WAIT_FOR_PROCESS = 10000; //ms
static const int TIME_UPDATE_AIRODUMP = 150; //ms
static const int MAX_BSSID_CON = 700;
static const int MAX_ESSID = 500;

static const QString BUFFER_AIRODUMP_FILENAME   = "airodump.M4"; //same that in common.h. m4-Compiled aiodump-ng
static const QString AIRODUMP_COM               = "aircrack-ng-1.1-M4/airodump-ng";
static const QString AIREPLAY_COM               = "aircrack-ng-1.1-M4/aireplay-ng";
static const QString AIRCRACK_COM               = "aircrack-ng-1.1-M4/aircrack-ng";
static const QString REAVER_COM                 = "utils/reaver";

static const QString CAPTURE_FOLDER     = "captures/";
static const QString ARP_FOLDER         = "arp_replays/";
static const QString FRAG_FOLDER        = "frag_caps/";
static const QString CHOP_FOLDER        = "chop_caps/";
static const QString FORGED_ARPS        = "forged_arps/";
static const QString BROADCAST_FOLDER   = "broadcast_caps/";
static const QString REAVER_FOLDER      = "reaver_caps/";
static const QString LOGS_FOLDER        = "logs/";

static const QString MAIN_LOG = LOGS_FOLDER + "mainlog";

static const QStringList programsNeeded = QStringList()
        << AIRODUMP_COM
        << AIREPLAY_COM
        << AIRCRACK_COM
        << REAVER_COM
        << "packetforge-ng"
        << "airmon-ng"
        << "macchanger"
        << "tcpdump --help"; //--help because tcpdump never returns prompt

#endif
