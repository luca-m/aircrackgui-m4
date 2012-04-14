#ifndef INFOCONNECTIONBSSID_H
#define INFOCONNECTIONBSSID_H
#include <QString>
#include "utils.h"
#include "DEFINES.h"

class infoConnectionBssid
{
public:
    infoConnectionBssid();
    void setBSSID(QString B)            {BSSID=B;}
    void setStation(QString s)          {station=s;}
    void setPower(int p)                {power=p;}
    void setPower(QString p)            {power=p.toInt();}
    void setRate(QString b)             {rate=b;}
    void setLost(int d)                 {lost=d;}
    void setLost(QString d)             {lost=d.toInt();}
    void setPackets(int ds)             {packets=ds;}
    void setPackets(QString ds)         {packets=ds.toInt();}
    void setProbes(QString ds)          {probes=ds;}

    QString getBSSID()const         {return BSSID;}
    QString getStation()const       {return station;}
    int getPower()const             {return power;}
    QString getRate()const           {return rate;}
    int getLost()const              {return lost;}
    int getPackets()const             {return packets;}
    QString getProbes()const           {return probes;}


    int numberFields(){return 7;}
    void print(){utils::mostrarMensaje("BSSID = " + this->getBSSID() + " STATION = " + this->getStation());}

private:
    QString BSSID;
    QString station;
    int power;
    QString rate;
    int lost;
    int packets;
    QString probes;

};


class infoConnectionBssidV {

public:
    infoConnectionBssidV(){};
    ~infoConnectionBssidV(){};
    infoConnectionBssid& operator[](const int index);
    void clear();

private:
    infoConnectionBssid infoC[MAX_BSSID_CON];
};

#endif // INFOCONNECTIONBSSID_H
