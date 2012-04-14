#include "debug.h"

QStringList debug::data;


debug::debug()
{
}


void debug::add(QString com){
    /*
    if (GLOBALS::debug)
        fprintf(stderr, (utils::actualTime() + " " + com+"\n").toLatin1().data());
        */

    data.append(com);
}

void debug::clear(){
    data.clear();
}

void debug::printAll(){
    for (int i=0; i<data.size(); ++i) {
        //fprintf(stderr, data.at(i).toLatin1().data());
        fprintf(stderr, "\n");
    }
}
