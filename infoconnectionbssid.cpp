#include "infoconnectionbssid.h"

infoConnectionBssid::infoConnectionBssid()
{
}

infoConnectionBssid& infoConnectionBssidV::operator [](const int index) {
    if (index > MAX_BSSID_CON){
        utils::mostrarError("CRITICAL Error, index if infoConnectionV is more greater than vector");//here debug
        exit(-1);
    }

    return infoC[index];
}


void infoConnectionBssidV::clear(){
    for (int i=0;  i < MAX_BSSID_CON; ++i)
            infoC[i].setBSSID("");
}
