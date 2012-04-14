#ifndef ATTACKARPREPLAY_H
#define ATTACKARPREPLAY_H
#include "baseattack.h"

class attackArpReplay : public baseAttack
{
public:
    attackArpReplay();

    bool start(const QString &BSSID, QString MAC, QString cap = "");

};

#endif // ATTACKARPREPLAY_H
