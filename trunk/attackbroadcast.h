#ifndef ATTACKBROADCAST_H
#define ATTACKBROADCAST_H
#include "baseattack.h"
#include "GLOBALS.h"


class attackBroadcast : public baseAttack
{
public:
    attackBroadcast();

    bool start(const QString &BSSID, QString Mac);
};

#endif // ATTACKBROADCAST_H
