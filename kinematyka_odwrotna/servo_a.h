#ifndef servo_a_h
#define servo_a_h

#include "Arduino.h"
#include <VarSpeedServo.h>
#include <LiquidCrystal.h>
#include "silnik.h"
#include "lcd_angle.h"
#include "lcd_pos.h"

class cServoA : public cSilnik
{
  private: //wskazniki LCD sa dziedziczone
    double _dKPpart; //zwiększane kawałki od aktualnego punktu P do nowopodanego punktu P (w iteracjach)
    double _dKPtemp; //tymczasowy y,z,kp do obliczen (używane przy d_y_part, d_z_part, d_kp_part)

  public:
    //----------------------------------------------------------KONSTRUKTOR-------------------------------------------------------------//
    cServoA(int nPin, float fKatMin, float fKatMax, cLCD_angle* pLCD_angle, cLCD_pos* pLCD_pos, cKomunikacja* pKomunikacja)
    {
      _nPin = nPin;
      _fKatMin = fKatMin;
      _fKatMax = fKatMax;
      _pLCD_angle = pLCD_angle;
      _pLCD_pos = pLCD_pos;
      _nPredkosc = MOTOR_SPEED_NORMAL;
       _pKomunikacja = pKomunikacja;

      _dKPpart = 0;
      _dKPtemp = 0;
    }

    //------------------------------------------------------------METODY-------------------------------------------------------------//
    void ZwiekszKPpartTempem()                     { _dKPtemp += _dKPpart; }
    
    void Trash()
    {
      _pKomunikacja->PrzygotujOdpowiedzNaRdzen(sCoreCommand);
      this->UstawKat(175); //ustaw podstawe nad pundełkiem na zbite bierki...
      _pLCD_angle->PrintAngle("servoA", this->getKat() );
      _pLCD_pos->PrintPos("x", -1); //speed ustawiony na sztywno
      String sXYprzyResecie = "(170,240)"; //...i wyceluj tam na płaszczyźnie (y,z).
      return sXYprzyResecie;
    }

    //----------------------------------------------------METODY-DOSTĘPOWE-DO-PÓL----------------------------------------------------//
    int getKPpart() const                          { return _fKatMax; }
    int getKPtemp() const                          { return _dKPtemp; }

    void setKPpart(double dKPpart)                 { _dKPpart = dKPpart; }
    void setKPtemp(double dKPtemp)                 { _dKPtemp = dKPtemp; }
};

#endif
