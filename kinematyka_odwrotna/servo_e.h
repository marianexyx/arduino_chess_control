#ifndef servo_e_h
#define servo_e_h

#include "Arduino.h"
#include <VarSpeedServo.h>
#include <LiquidCrystal.h>
#include "silnik.h"
#include "lcd_angle.h"
#include "lcd_pos.h"
#include <math.h>

class cServoE : public cSilnik
{
  private: //wskazniki LCD sa dziedziczone
    
  public:
    //----------------------------------------------------------KONSTRUKTOR----------------------------------------------------------//
    cServoE(int nPin, float fKatMin, float fKatMax, cLCD_angle* pLCD_angle, cKomunikacja* pKomunikacja)
    {
      _nPin = nPin;
      _fKatMin = fKatMin;
      _fKatMax = fKatMax;
      _pLCD_angle = pLCD_angle;
      _nPredkosc = MOTOR_SPEED_NORMAL;
      _pKomunikacja = pKomunikacja;
    }
};

#endif
