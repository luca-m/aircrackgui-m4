#ifndef ATTACKAUTH_H
#define ATTACKAUTH_H
#include "baseattack.h"
#include "DEFINES.h"
#include "utils.h"
#include "GLOBALS.h"


class attackAuth : public baseAttack
{
public:
    attackAuth();

    bool start(const QString &BSSID, QString Mac);
};

#endif // ATTACKAUTH_H
