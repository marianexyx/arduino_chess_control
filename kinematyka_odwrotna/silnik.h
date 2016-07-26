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

class cSilnik
{    
  protected:
    int _nPin;
    int _nPredkosc;
    float _fKat; 
    float _fKatMin;
    float _fKatMax;
    VarSpeedServo _Servo;
    cLCD_angle* _pLCD_angle; //ustanowienie relacji zawierania z obiektem lcd
    cLCD_pos* _pLCD_pos;
    
 public:
    //----------------------------------------------------------KONSTRUKTORY---------------------------------------------------------//
    cSilnik() {} //dla dziedziczenia
    cSilnik(int nPin, float fKatMin, float fKatMax, cLCD_angle* pLCD_angle, cLCD_pos* pLCD_pos);

    //------------------------------------------------------------METODY-------------------------------------------------------------//
    void Rozpocznij();
    void UstawKat(float fKat, MOTOR_SPEED predkoscSilnika);    //podstawowy ruch - automatyczny false
    void UstawKat(float fKat, MOTOR_SPEED predkoscSilnika, bool bCzekajNaKoniecRuchu);  //podstawowy ruch - w domysle do wrzucania true
    void UstawKatSerwisowo(String sNazwaKata, String sKat); // serwisowe alpha beta ks fi   
    
    //----------------------------------------------------METODY-DOSTĘPOWE-DO-PÓL----------------------------------------------------//
    float getKat() const                           { return _fKat; }
    int getPredkosc() const                        { return _nPredkosc; }
    int getKatMin() const                          { return _fKatMin; }
    int getKatMax() const                          { return _fKatMax; }

    void setPredkosc(MOTOR_SPEED predkoscSilnika)  { _nPredkosc = predkoscSilnika; } 
};

#endif
