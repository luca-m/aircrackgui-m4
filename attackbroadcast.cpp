#include "attackbroadcast.h"

attackBroadcast::attackBroadcast()
{
}


bool attackBroadcast::start(const QString &BSSID, QString MAC)
{
    if (!utils::validMAC(BSSID)){
        utils::mostrarError("Invalid BSSID");
        return false;
    }

    if (!MAC.isEmpty()) {
        if (!utils::validMAC(BSSID)){
                utils::mostrarError("Invalid MAC");
                return false;
        }

        else
            MAC = " -h " + MAC; //add flag to mac
    }

    const QString command = "../../" + AIREPLAY_COM +
                            " -2 -p 0841 -c FF:FF:FF:FF:FF:FF " +
                            " -m " + QString::number(GLOBALS::BROADCAST_MIN_PACKET) +
                            " -n " + QString::number(GLOBALS::BROADCAST_MAX_PACKET) +
                            " -b " + BSSID +
                            MAC +
                            " -x" + QString::number(GLOBALS::SEND_RATE) + " " + GLOBALS::INTERFACE;

    return startCommand(command);
}
