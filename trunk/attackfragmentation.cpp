#include "attackfragmentation.h"

attackFragmentation::attackFragmentation()
{
}


bool attackFragmentation::start(const QString &BSSID, QString MAC)
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
            " -5 " +
            " -m " + QString::number(GLOBALS::FRAG_MIN_PACKET) +
            " -n " + QString::number(GLOBALS::FRAG_MAX_PACKET) +
            " -b " + BSSID +
            MAC +
            " " + GLOBALS::INTERFACE;

    return startCommand(command);
}
