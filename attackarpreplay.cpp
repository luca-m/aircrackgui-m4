#include "attackarpreplay.h"
#include "GLOBALS.h"

attackArpReplay::attackArpReplay()
{
}


bool attackArpReplay::start(const QString &BSSID, QString MAC, QString cap)
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

    //if cap is empty, there are no cap to use
    if (!cap.isEmpty())
        //cap have to be in source path of program
        cap = cap.prepend(" -r ");

    const QString command = "../../" + AIREPLAY_COM + " -3 -b "
                            + BSSID + MAC + cap + " -x" + QString::number(GLOBALS::SEND_RATE) + " " + GLOBALS::INTERFACE;

    return startCommand(command);
}
