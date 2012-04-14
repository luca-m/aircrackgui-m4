#include "attackdeauth.h"

attackDeauth::attackDeauth()
{
}

bool attackDeauth::start(const QString &BSSID, QString MAC)
{
    if (this->isRunning())
        this->stop();

    if (!utils::validMAC(BSSID)){
        utils::mostrarError("Invalid BSSID");
        this->stop();
        return false;
    }

    if (!MAC.isEmpty()) {
        if (!utils::validMAC(BSSID)){
                utils::mostrarError("Invalid MAC DESTINATION");
                this->stop();
                return false;
        }

        else
            MAC = " -c " + MAC; //add flag to mac
    }

    const QString command = AIREPLAY_COM + " -0 10 -a " +
            BSSID + MAC + " " + GLOBALS::INTERFACE;

    return startCommand(command);

}
