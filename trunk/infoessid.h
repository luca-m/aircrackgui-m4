#ifndef INFOESSID_H
#define INFOESSID_H
#include <QString>
#include "utils.h"
class infoESSID
{
public:
    infoESSID();
    ~infoESSID();

    void setBSSID(QString B)            {BSSID=B;}
    void setPower(int p)                {power=p;}
    void setPower(QString p)            {power=p.toInt();}
    void setBeacons(int b)              {beacons=b;}
    void setBeacons(QString b)          {beacons=b.toInt();}
    void setData(int d)                 {data=d;}
    void setData(QString d)             {data=d.toInt();}
    void setDataS(int ds)               {dataS=ds;}
    void setDataS(QString ds)           {dataS=ds.toInt();}
    void setChannel(int ch)             {channel=ch;}
    void setChannel(QString ch)         {channel=ch.toInt();}
    void setMb(QString m)               {mb=m;}
    void setEnc(QString e)              {enc=e;}
    void setCipher(QString c)           {cipher=c;}
    void setAuth(QString a)             {auth=a;}
    void setName(QString n)             {name=n;}

    QString getBSSID()const         {return BSSID;}
    int getPower()const             {return power;}
    int getPowerConverted()const             {return ((power != -1) ? 100-abs(power) : 0);}
    int getBeacons()const           {return beacons;}
    int getData()const              {return data;}
    int getDataS()const             {return dataS;}
    int getChannel()const           {return channel;}
    QString getMb()const                {return mb;}
    QString getEnc()const           {return enc;}
    QString getCipher()const        {return cipher;}
    QString getAuth()const          {return auth;}
    QString getName()const          {return name;}


    int numberFields(){return 11;}
    void print(){utils::mostrarMensaje("BSSID = " + this->getBSSID() + " POWER = " + QString::number(this->getPowerConverted()));}

    static void swap(infoESSID &a, infoESSID &b){infoESSID &temp = a; a = b; b = temp;}

private:
    QString BSSID;
    int power;
    int beacons;
    int data;
    int dataS;
    int channel;
    QString mb;
    QString enc;
    QString cipher;
    QString auth;
    QString name;


};

class infoESSIDV {

public:
    infoESSIDV(){};
    ~infoESSIDV(){};
    infoESSID& operator[](const int index);
    void clear();
    void sort();
    infoESSID* get(QString BSSID);

private:
    infoESSID infoE[MAX_ESSID];

};
#endif // INFOESSID_H
