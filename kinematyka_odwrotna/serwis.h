#ifndef serwis_h
#define serwis_h

#include "Arduino.h"
#include "silnik.h"
#include "komunikacja.h"
#include "ramie.h"

#define SERVO_POWER_PIN1 50 //zamiast pinów posługujemy się zdefiniowanymi słowami
#define SERVO_POWER_PIN2 51

class cSerwis
{
  private:
    cKomunikacja* _pKomunikacja;
    cRamie* _pRamie;
    int _nPredkoscSerwisowa; // program co zakończenie ruchu ramieniem wraca prędkoscią poruszania ramienia do stałej normalnej prędkosci. jeżeli ręcznie zmienimy tą ...
    //...prędkość funkcją serwisową "speed =" to program będzie utrzymywał prędkość tej tutaj zmiennej. na starcie ustawione jest 0, by omijać tą zmienną, jak nic nie jest zmienione.
    
  public:
    //----------------------------------------------------------KONSTRUKTOR----------------------------------------------------------//
    cSerwis(cKomunikacja* pKomunikacja, cRamie* pRamie); 

    //------------------------------------------------------------METODY-------------------------------------------------------------//
    void ZmienPredkosc(String sKomendaRdzenia = "speed = 18"); //ze stringu wyjmowana jest tylko liczba
    void WlaczSerwa();
    void WylaczSerwa();
    void WlaczInfo();
    void WylaczInfo();

    //----------------------------------------------------METODY-DOSTĘPOWE-DO-PÓL----------------------------------------------------//
    int getPredkoscSerwisowa() const                          { return _nPredkoscSerwisowa; }
};

#endif
