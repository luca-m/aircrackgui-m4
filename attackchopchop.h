#ifndef ATTACKCHOPCHOP_H
#define ATTACKCHOPCHOP_H
#include "baseattack.h"
#include "GLOBALS.h"


class attackChopChop : public baseAttack
{
public:
    attackChopChop();

    bool start(const QString &BSSID, QString MAC);
};

#endif // ATTACKCHOPCHOP_H
