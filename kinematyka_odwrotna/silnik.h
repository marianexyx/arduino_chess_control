#ifndef silnik_h
#define silnik_h

#include "Arduino.h"
#include <VarSpeedServo.h>
#include <LiquidCrystal.h>
#include "lcd_angle.h"
#include "lcd_pos.h"
#include "komunikacja.h"
#include "ramie.h"

enum MOTOR_SPEED { MOTOR_SPEED_SLOW  = 2, MOTOR_SPEED_NORMAL = 18, MOTOR_SPEED_FAST  = 70, MOTOR_SPEED_MAX   = 255 };

class cLCD_angle;

class cSilnik
{    
  protected:
    int _nPin;
    // int _nPredkosc; // prędkosc zawsze(?) powinno ciagnac z ramienia
    String _sNazwaKata;
    float _fKat; 
    float _fKatMin;
    float _fKatMax;
    VarSpeedServo _Servo;
    cLCD_angle* _pLCD_angle; 
    cLCD_pos* _pLCD_pos;
    cRamie* _pRamie
    cKomunikacja* _pKomunikacja
    
 public:
    //----------------------------------------------------------KONSTRUKTORY---------------------------------------------------------//
    cSilnik() {} //dla dziedziczenia wymagany jest pusty konstruktor (?)
    cSilnik(int nPin, String sNazwaKata, float fKatMin, float fKatMax, cLCD_angle* pLCD_angle, cLCD_pos* pLCD_pos, cRamie* pRamie, cKomunikacja* pKomunikacja); // ! jezeli to ma być klasa wirtualna, to ten konstruktor zbędny, jako że nigdy nie będzie twrozony

    //------------------------------------------------------------METODY-------------------------------------------------------------//
    void Rozpocznij();
    void UstawKat(float fKat, int nPedkoscServa = _pRamie->getPredkosc(), bool bCzekajNaKoniecRuchu = false); // !! nie widze szans by ta sztuczka ze wskaznikiem sie udala - uzyc static
    void UstawKatSerwisowo(String sNazwaKata, String sKat); // serwisowe alpha beta ks fi   !!! wyjebalem tutaj stad te wartosci
    
    //----------------------------------------------------METODY-DOSTĘPOWE-DO-PÓL----------------------------------------------------//
    float getKat() const                           { return _fKat; }
    String getNazwaKata() const                    { return _sNazwaKata; }
    int getPredkosc() const                        { return _nPredkosc; }
    int getKatMin() const                          { return _fKatMin; }
    int getKatMax() const                          { return _fKatMax; }

    void setPredkosc(int nPredkosc)                { _nPredkosc = nPredkosc; } // ! powinno być używane tylko w dziedziczeniu - virtual?
};

#endif
