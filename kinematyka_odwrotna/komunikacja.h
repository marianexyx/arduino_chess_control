#ifndef komunikacja_h
#define komunikacja_h

#include "Arduino.h"

class cServoA;
class cServoB;
class cServoC;
class cServoD;
class cSilnik;
//class cRamie;

enum INFO_TYPE {INFO_FIELD_LETTER,
                INFO_FIELD_NUMBER,
                /*INFO_KAT_KP,
                INFO_ODLEGLOSC_Y,
                INFO_ODLEGLOSC_Z,
                INFO_ODLEGLOSC_Z_UPGR,
                INFO_KAT_ALPHA,
                INFO_KAT_BETA,
                INFO_KAT_FI,*/
                INFO_OPEN,
                INFO_CLOSE,
                INFO_UP,
                INFO_DOWN,
                INFO_ELSE
               };

class cKomunikacja
{
  private:
    String _sInfoDlaRdzenia; //zmienna po której komunikuje się arduino z core'em, tj. mówi mu co aktualnie za żądanych ruch wykonał.
    bool _bZnakiKoncowLinii; //pokazuj znaki początku i końca linii w wiadomościach do core
    bool _bSekwencjaRuchow; //definiuje czy ruch jest spowodowany ręcznie (false), czy przez core (true). wartości core'a zmieniają tą zmienną.
    bool _bPokazujeInfo; // jeżeli = true, to wypluwa na serial port tonę informacji o swoim stanie...
    //...podczas działania. by działać z core'm musi być ustawione na false.
    
  public:
    //----------------------------------------------------------KONSTRUKTOR----------------------------------------------------------//
    cKomunikacja(); // ustawi pokazywanie info automatycznie na false

    //------------------------------------------------------------METODY-------------------------------------------------------------//
    void PrzygotujOdpowiedzNaRdzen(String sPolecenieRdzenia);
    void OdpowiedzNaRdzen(String sMsgPart);
    void OdpowiedzNaRdzen(String sMsgPart1, String sMsgPart2);
    void ServoPozaZakresem();
    void PokazInfo(double dZmiennaDoPokazania, INFO_TYPE TypInformacji = INFO_ELSE);

    //----------------------------------------------------METODY-DOSTĘPOWE-DO-PÓL----------------------------------------------------//
    String InfoDlaRdzenia() const                            { return _sInfoDlaRdzenia; }
    bool getSekwencjaRuchow() const                          { return _bSekwencjaRuchow; }
    bool getZnakiKoncowLinii() const                         { return _bZnakiKoncowLinii; }
    bool CzyPokazujeInfo() const                             { return _bPokazujeInfo; }

    //gdy pokazujemy informacje o systemie to nie pokazujemy znaków komunikacji między systemami, tj. znaki początku i końca wiadomośći. i na odwrót
    void PokazujInfo(bool bWlaczoneInfo)                     { _bPokazujeInfo = bWlaczoneInfo; this->setZnakiKoncowLinii(!bWlaczoneInfo); } 
    void UstawWiadomoscDlaRdzenia(String sInfoDlaRdzenia)    { _sInfoDlaRdzenia = sInfoDlaRdzenia; }
    void setSekwencjaRuchow(bool bSekwencjaRuchow)           { _bSekwencjaRuchow = bSekwencjaRuchow; }
    void setZnakiKoncowLinii(bool bZnakiKoncowLinii)         { _bZnakiKoncowLinii = bZnakiKoncowLinii; }
};

#endif
