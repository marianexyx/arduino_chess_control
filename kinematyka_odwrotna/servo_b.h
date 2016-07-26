#ifndef servo_b_h
#define servo_b_h

#include "Arduino.h"
#include <VarSpeedServo.h>
#include <LiquidCrystal.h>
#include "silnik.h"
#include "lcd_angle.h"
#include "lcd_pos.h"
#include <math.h>

class cServoB : public cSilnik
{
  private: //wskazniki LCD sa dziedziczone
    
  public:
    //----------------------------------------------------------KONSTRUKTOR----------------------------------------------------------//
    cServoB(int nPin, float fKatMin, float fKatMax, cLCD_angle* pLCD_angle)
    {
      _nPin = nPin;
      _fKatMin = fKatMin;
      _fKatMax = fKatMax;
      _pLCD_angle = pLCD_angle;
    }

    //------------------------------------------------------------METODY-------------------------------------------------------------//
    void ObliczKatAlfa(double dL, double dA, double dB, int nYtemp)
    {
      //pre_alpha 1 i 2 pomocnicze przy liczeniu kątów serw (problem z zasięgiem zmiennych)
      double dPreAlpha1Rad = (dL * dL + dA * dA - dB * dB) / (2 * dL * dA); //obliczanie alpha1 w radianach bez acos
      double dPreAlpha2Rad = nYtemp / dL; //obliczanie alpha2 w radianach bez acos
      double dAlphaRad = acos(dPreAlpha1Rad) + acos(dPreAlpha2Rad); //cały kąt alpha w radianach
      double dAlpha = (180 / PI) * dAlphaRad; //docelowy kąt alpha
      _fKat = dAlpha;

      _pKomunikacja->PokazInfo(INFO_KAT_ALPHA, (String)_fKat);
    }
    void PodniesPrewencyjnie()  //prewencyjne odsunięcie od planszy
    {
      this->write(_fKat + 8, MOTOR_SPEED_NORMAL, true); //!!!! normalnie predkosc = 6
      _pLCD_angle->PrintAngle("servoB", _fKat);
    }
};

#endif
