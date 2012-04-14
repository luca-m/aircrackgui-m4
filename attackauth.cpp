#include "attackauth.h"
#include "GLOBALS.h"

attackAuth::attackAuth()
{
}

bool attackAuth::start(const QString &BSSID, QString MAC)
{
    //if we launch many times since airodump attack auth, stop previous process
    if (this->isRunning())
        this->stop();


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

    const QString command = AIREPLAY_COM +
                            " -1 30 -a " + BSSID +
                            MAC +
                            " -o " + QString::number(GLOBALS::AUTH_PACK_BURST) +
                            " -T " + QString::number(GLOBALS::AUTH_RETRIES) +
                            " " + GLOBALS::INTERFACE;


    return startCommand(command);
}
