#ifndef DEBUG_H
#define DEBUG_H
#include "QString"
#include "qstringlist.h"
#include "GLOBALS.h"
#include "stdio.h"
#include "utils.h"

class debug
{

public:
    debug();

private:

    static QStringList data;

public:
    static QStringList const get() {return data;}
    static void printAll();
    static void clear();
    static void add(QString com);


};

#endif // DEBUG_H
