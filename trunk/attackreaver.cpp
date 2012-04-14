#include "attackreaver.h"

attackReaver::attackReaver()
{
}


bool attackReaver::start(const QString &BSSID, const QString session)
{
    if (!utils::validMAC(BSSID)){
        utils::mostrarError("Invalid BSSID");
        return false;
    }

    const QString command = "../../" + REAVER_COM + session + " -vv -a -b " + BSSID + " -i " + GLOBALS::INTERFACE;

    return startCommand(command);
}
