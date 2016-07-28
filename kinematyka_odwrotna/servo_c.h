#ifndef servo_c_h
#define servo_c_h

#include "Arduino.h"
#include <VarSpeedServo.h>
#include <LiquidCrystal.h>
#include "silnik.h"
#include "lcd_angle.h"
#include "lcd_pos.h"
#include "komunikacja.h"
#include <math.h>

class cServoC : public cSilnik
{
  private: //wskazniki LCD sa dziedziczone

  public:
    //----------------------------------------------------------KONSTRUKTOR----------------------------------------------------------//
    cServoC(int nPin, float fKatMin, float fKatMax, cLCD_angle* pLCD_angle, cKomunikacja* pKomunikacja)
    {
      _nPin = nPin;
      _fKatMin = fKatMin;
      _fKatMax = fKatMax;
      _pLCD_angle = pLCD_angle;
      _nPredkosc = MOTOR_SPEED_NORMAL;
      _pKomunikacja = pKomunikacja;
    }

    //------------------------------------------------------------METODY-------------------------------------------------------------//
    //servo beta dziala w przedziale od 24 do 157 stopni. 
    
    void ObliczKatBeta(double dL, double dA, double dB, cKomunikacja* pKomunikacja)
    {  
      double dBetaRad = acos((dA * dA + dB * dB - dL * dL) / (2 * dA * dB)); //obliczanie bety w radianach
      _fKat = (180 / PI) * dBetaRad; //kąt beta docelowy.

      if (_fKat >= 157) _fKat = 156; //!!servo beta ma słaby zakres!!!

      pKomunikacja->PokazInfo(INFO_KAT_BETA, (String)_fKat);
    }
};

#endif
