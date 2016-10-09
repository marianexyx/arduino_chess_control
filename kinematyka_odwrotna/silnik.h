#ifndef silnik_h
#define silnik_h

#include "Arduino.h"
#include <VarSpeedServo.h>
#include <LiquidCrystal.h>
#include "lcd_angle.h"
#include "lcd_pos.h"
#include "komunikacja.h"


enum MOTOR_SPEED { MOTOR_SPEED_SLOW  = 2, MOTOR_SPEED_NORMAL = 18, MOTOR_SPEED_FAST  = 70, MOTOR_SPEED_MAX   = 255 };

class cLCD_angle;

class cSilnik //virtual?
{    
  protected:
    int _nPin;
    static int _nPredkosc; 
    String _sNazwaKata;
    double _dKat; 
    int _nKatMin;
    int _nKatMax;
    VarSpeedServo _Servo;
    cLCD_angle* _pLCD_angle; 
    cLCD_pos* _pLCD_pos;

 public:
    //----------------------------------------------------------KONSTRUKTORY---------------------------------------------------------//
    cSilnik() {} //dla dziedziczenia wymagany jest pusty konstruktor (?)
    //cSilnik(int nPin, String sNazwaKata, float fKatMin, float fKatMax, cLCD_angle* pLCD_angle, cLCD_pos* pLCD_pos, cKomunikacja* pKomunikacja); // ! jezeli to ma być klasa wirtualna, to ten konstruktor zbędny, jako że nigdy nie będzie twrozony

    //------------------------------------------------------------METODY-------------------------------------------------------------//
    void Rozpocznij();
    void UstawKat(double dKat, int nPedkoscServa = _nPredkosc, bool bCzekajNaKoniecRuchu = false); 
    /*virtual void UstawKatSerwisowo() = 0;*/ // serwisowe alpha beta ks fi  - ! jak wlacze ta metoda jako wirtualna, to jego pochodne tez mi arduino krzyczy jako wirtualne i nie moge stworzyc obiektu z tych klas 
    
    //----------------------------------------------------METODY-DOSTĘPOWE-DO-PÓL----------------------------------------------------//
    double getKat() const                          { return _dKat; }
    String getNazwaKata() const                    { return _sNazwaKata; }
    int getPredkosc() const                        { return _nPredkosc; }
    int getKatMin() const                          { return _nKatMin; }
    int getKatMax() const                          { return _nKatMax; }

    void setPredkosc(int nPredkosc)                { _nPredkosc = nPredkosc; } // ! powinno być używane tylko w dziedziczeniu - virtual?
};

#endif
