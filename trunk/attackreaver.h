#ifndef ATTACKREAVER_H
#define ATTACKREAVER_H
#include "baseattack.h"
#include "GLOBALS.h"

class attackReaver : public baseAttack
{
public:
    attackReaver();

    bool start(const QString &bssid, const QString session="");
};

#endif // ATTACKREAVER_H
