#ifndef ATTACKFRAGMENTATION_H
#define ATTACKFRAGMENTATION_H
#include "baseattack.h"
#include "GLOBALS.h"


class attackFragmentation : public baseAttack
{
public:
    attackFragmentation();

    bool start(const QString &BSSID, QString MAC);
};

#endif // ATTACKFRAGMENTATION_H
