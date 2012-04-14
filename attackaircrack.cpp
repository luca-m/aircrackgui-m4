#include "attackaircrack.h"

attackAircrack::attackAircrack()
{
}


bool attackAircrack::start(const QString &BSSID)
{
    //if we launch many times since airodump attack auth, stop previous process
    if (isRunning())
        stop();

    if (!utils::validMAC(BSSID)){
        utils::mostrarError("Invalid BSSID");
        return false;
    }


    //obtaining files
    QString files;
    int left = 0, right = 1;
    do {

        if (QFile::exists(CAPTURE_FOLDER + BSSID +"-"+QString::number(left)+QString::number(right)+".cap")) {
            files += BSSID + "-"+QString::number(left)+QString::number(right)+".cap" + " ";
            if (left == 9 && right == 9) {
                utils::mostrarWarning(utils::htmlRojo("Maximum .cap files readed: 99. From X-99.cap to ahead we could not read data"));
            }
        }


        right++;
        if (right == 10) {
            right = 0;
            left++;
        }



    }   while (left != 10); //100 max

    const QString command = "../" + AIRCRACK_COM +
            " -b " + BSSID +
            " -f" + QString::number(GLOBALS::BRUTEFORCE_FACTOR) +
            + " " + files;

    process->setWorkingDirectory(CAPTURE_FOLDER);

    return startCommand(command);

}
