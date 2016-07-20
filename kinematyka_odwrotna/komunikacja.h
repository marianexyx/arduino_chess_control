#ifndef komunikacja_h
#define komunikacja_h

#include "Arduino.h"

enum INFO_TYPE {INFO_FIELD_LETTER,
                INFO_FIELD_NUMBER,
                INFO_KAT_KP,
                INFO_ODLEGLOSC_Y,
                INFO_ODLEGLOSC_Z,
                INFO_ODLEGLOSC_Z_UPGR,
                INFO_KAT_ALPHA,
                INFO_KAT_BETA,
                INFO_KAT_FI,
                INFO_OPEN,
                INFO_CLOSE,
                INFO_UP,
                INFO_DOWN
               };

class cKomunikacja
{
  private:
    bool _bPokazujeInfo; // jeżeli = true, to wypluwa na serial port tonę informacji o swoim stanie 
    //podczas działania. by działać z core'm musi być ustawione na false.
    String _sInfoDlaRdzenia; //zmienna po której komunikuje się arduino z core'em, tj. mówi mu co aktualnie za żądanych ruch wykonał.
    bool _bZnakiKoncowLinii; //pokazuj znaki początku i końca linii w wiadomościach do core
    bool _bSekwencjaRuchow; //definiuje czy ruch jest spowodowany ręcznie (false), czy przez core (true). wartości core'a zmieniają tą zmienną.
    
  public:
    // konstruktor
    cKomunikacja(); // ustawi pokazywanie info automatycznie na false
    // -----------------------------------------------------

    // metody
    void PrzygotujOdpowiedzNaRdzen(String sPolecenieRdzenia);
    void PokazInfo(INFO_TYPE typInformacji, String sZmiennaDoPokazania);
    void OdpowiedzNaRdzen(String sMsgPart);
    void OdpowiedzNaRdzen(String sMsgPart1, String sMsgPart2);
    void ServoPozaZakresem();
    //------------------------------------------------------

    // metody dostępowe do pól
    bool CzyPokazujeInfo() const                             { return _bPokazujeInfo; }
    String InfoDlaRdzenia() const                            { return _sInfoDlaRdzenia; }
    bool getSekwencjaRuchow() const                          { return _bSekwencjaRuchow; }

    void PokazujInfo(bool bWlaczoneInfo)                     { _bPokazujeInfo = bWlaczoneInfo; _bZnakiKoncowLinii = !bWlaczoneInfo; }
    void UstawWiadomoscDlaRdzenia(String sInfoDlaRdzenia)    { _sInfoDlaRdzenia = sInfoDlaRdzenia; }
    void setSekwencjaRuchow(bool bSekwencjaRuchow)           { _bSekwencjaRuchow = bSekwencjaRuchow; }
};

#endif
