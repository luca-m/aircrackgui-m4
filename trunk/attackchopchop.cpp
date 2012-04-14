#include "attackchopchop.h"

attackChopChop::attackChopChop()
{
}

bool attackChopChop::start(const QString &BSSID, QString MAC)
{
    if (!utils::validMAC(BSSID)){
        utils::mostrarError("Invalid BSSID");
        return false;
    }

    //else
    if (!MAC.isEmpty()) {

        if (!utils::validMAC(BSSID)){
            utils::mostrarError("Invalid MAC");
            return false;
        }

        else
            MAC = " -h " + MAC; //add flag to mac
    }

    const QString command = "../../" + AIREPLAY_COM +
            " -4 " +
            " -m " + QString::number(GLOBALS::CHOP_MIN_PACKET) +
            " -n " + QString::number(GLOBALS::CHOP_MAX_PACKET) +
            " -b " + BSSID +
            MAC +
            " " + GLOBALS::INTERFACE;

    return startCommand(command);
}
