#ifndef GLOBALS_H
#define GLOBALS_H
#include <QString>
#include <logthread.h>


class GLOBALS {
public:
    //Main Interface
    static QString INTERFACE;
    //Packets/s
    static int SEND_RATE;
    //Bruteforce fudge factor
    static int BRUTEFORCE_FACTOR;
    //number of packets per burst in auth
    static int AUTH_PACK_BURST;
    //exit after retry fake auth request
    static int AUTH_RETRIES;
    //minimum packet length
    static int BROADCAST_MIN_PACKET;
    static int CHOP_MIN_PACKET;
    static int FRAG_MIN_PACKET;
    //maximum packet length
    static int BROADCAST_MAX_PACKET;
    static int CHOP_MAX_PACKET;
    static int FRAG_MAX_PACKET;
};
#endif // GLOBALS_H
