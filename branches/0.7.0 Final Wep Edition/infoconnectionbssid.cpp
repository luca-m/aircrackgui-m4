#include "infoconnectionbssid.h"

infoConnectionBssid::infoConnectionBssid()
{
}

infoConnectionBssid& infoConnectionBssidV::operator [](const int index) {
    if (index > MAX_BSSID_CON){
        utils::mostrarError("Error, index if infoConnectionV is more greater than vector");//here debug
        infoConnectionBssid temp;
        return temp;
    }

    return infoC[index];
}


void infoConnectionBssidV::clear(){
    for (int i=0;  i < MAX_BSSID_CON; ++i)
            infoC[i].setBSSID("");
}
