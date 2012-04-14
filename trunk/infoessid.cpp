#include "infoessid.h"

infoESSID::infoESSID()
{
}

infoESSID::~infoESSID()
{
}

infoESSID* infoESSIDV::get(QString BSSID){
    for (int i=0; i < MAX_ESSID; ++i)
        if (infoE[i].getBSSID() == BSSID)
            return &infoE[i];

    return NULL;
}

infoESSID& infoESSIDV::operator [](const int index) {
    if (index > MAX_ESSID){
        utils::mostrarError("CRITICAL Error, index if infoConnectionV is more greater than vector");//here debug
        exit(-1);
    }

    return infoE[index];
}

void infoESSIDV::clear(){
    for (int i=0; i < MAX_ESSID; ++i)
            infoE[i].setBSSID("");
}

void infoESSIDV::sort(){
    //bubble method, can be improved
    for (int i=0; i < MAX_ESSID; ++i)
        for (int j=1; j < MAX_ESSID; ++j)
            if (infoE[j].getPowerConverted() > infoE[j-1].getPowerConverted())
                infoESSID::swap(infoE[j], infoE[j-1]);
}
