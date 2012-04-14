#ifndef ATTACKAIRCRACK_H
#define ATTACKAIRCRACK_H
#include "baseattack.h"
#include "GLOBALS.h"


class attackAircrack : public baseAttack
{
public:
    attackAircrack();

    bool start(const QString &BSSID);
};

#endif // ATTACKAIRCRACK_H
