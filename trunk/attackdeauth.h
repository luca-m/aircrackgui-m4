#ifndef ATTACKDEAUTH_H
#define ATTACKDEAUTH_H
#include <QString>
#include "baseattack.h"
#include "GLOBALS.h"


class attackDeauth : public baseAttack
{
public:
    attackDeauth();

    bool start(const QString &BSSID, QString MAC);
};

#endif // ATTACKDEAUTH_H
