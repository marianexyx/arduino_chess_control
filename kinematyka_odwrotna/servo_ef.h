#ifndef servo_ef_h
#define servo_ef_h

#include "Arduino.h"
#include <VarSpeedServo.h>
#include <LiquidCrystal.h>
#include "silnik.h"
#include "lcd_angle.h"
#include "lcd_pos.h"
#include <math.h>

class cServoEF : public cSilnik
{
  private: //wskazniki LCD sa dziedziczone
    
  public:
    //----------------------------------------------------------KONSTRUKTOR----------------------------------------------------------//
    cServoEF(int nPin, String sNazwaKata, int nKatMin, int nKatMax, cLCD_angle* pLCD_angle, cLCD_pos* pLCD_pos, cKomunikacja* pKomunikacja)
    {
      _nPin = nPin;
      _sNazwaKata = sNazwaKata;
      _nKatMin = nKatMin;
      _nKatMax = nKatMax;
      _pLCD_angle = pLCD_angle;
      _nPredkosc = MOTOR_SPEED_NORMAL;
      _pKomunikacja = pKomunikacja;
    }
};

#endif
