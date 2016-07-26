#ifndef serwis_h
#define serwis_h

#include "Arduino.h"
#include "silnik.h"
#include "komunikacja.h"

#define SERVO_POWER_PIN1 50 //zamiast pinów posługujemy się zdefiniowanymi słowami
#define SERVO_POWER_PIN2 51

class cSerwis
{
  private:
    cKomunikacja* _pKomunikacja;
    cSilnik* _pSilnik;
    
  public:
    //----------------------------------------------------------KONSTRUKTOR----------------------------------------------------------//
    cSerwis(cKomunikacja* pKomunikacja, cSilnik* pSilnik); 

    //------------------------------------------------------------METODY-------------------------------------------------------------//
    void ZmienPredkosc(String sKomendaRdzenia);
    void WlaczSerwa();
    void WylaczSerwa();
    void WlaczInfo();
    void WylaczInfo();
};

#endif
