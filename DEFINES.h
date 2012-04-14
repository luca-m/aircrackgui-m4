#ifndef DEFINES_H
#define DEFINES_H
#include "qstring.h"
///////////////////////////GENERAL///////////////////////////////////////////////////////////////////////////////////////////
static const int _VERSION = 0;
static const int _SUBVERSION = 7;
static const int _MIN_VERSION = 0;
static const QString _STATUS_VERSION = "Final WEP Edition";
static const QString ABOUT_THIS_PROGRAM =
"GUI for monitor wireless. Front-end of Aircrack-ng suite. Includes\
 others wireless tools like macchanger.\n\
This program has been created to increase performance and accesibility\
to the controlling and scanning wireless networks.\n\n\
Programmer: Manuel Garcia Sanz (supremo12345)\n\
Contact: manuelgs88@gmail.com\n\n\
Project Page: http://code.google.com/p/aircrackgui-m4/ \n\n\
Thx to:\n\
http://foro.seguridadwireless.net/\n\
USUARIONUEVO\n\
pac0\n\
sanson\n\
drvalium\n\
";
static const QString PROJECT_PAGE = "http://code.google.com/p/aircrackgui-m4/";
static const QString REPORT_BUG_PAGE = "http://code.google.com/p/aircrackgui-m4/issues/entry";
static const QString VERSION = QString::number(_VERSION) + "." + QString::number(_SUBVERSION) + "." + QString::number(_MIN_VERSION) + " " + _STATUS_VERSION;
static const QString PROGRAM_NAME = "Aircrack-ng [M4-GUI-Version] Linux " + VERSION;
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////DEFINES//////////////////////////////////////////////////////////////////////////////////////////
static const int TIME_AUTO_CHECK_UPDATES = 1800000; //ms (half hour)
static const int WAIT_FOR_PROCESS = 10000; //ms
static const int TIME_UPDATE_AIRODUMP = 200; //ms
static const int MAX_BSSID_CON = 700;
static const int MAX_ESSID = 500;
static const QString BUFFER_AIRODUMP_FILENAME = "airodump.M4"; //same that in common.h
static const QString AIRODUMP_COM = "aircrack-ng-1.1-M4/src/airodump-ng";
static const QString AIREPLAY_COM = "aircrack-ng-1.1-M4/src/aireplay-ng";
static const QString AIRCRACK_COM = "aircrack-ng-1.1-M4/src/aircrack-ng";
static const QString MACCHANGER_COM = "binutils/macchanger";
static const QString AIRMON_COM =   "airmon-ng";
static const QString CAPTURE_FOLDER = "captures/";
static const QString ARP_FOLDER = "arp_replays/";
static const QString FRAG_FOLDER = "frag_caps/";
static const QString CHOP_FOLDER = "chop_caps/";
static const QString FORGED_ARPS = "forged_arps/";
static const QString BROADCAST_FOLDER = "broadcast_caps/";
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



#endif
