#ifndef servo_a_h
#define servo_a_h

#include "Arduino.h"
#include <VarSpeedServo.h>
#include <LiquidCrystal.h>
#include "silnik.h"
#include "lcd_angle.h"
#include "lcd_pos.h"
#include "komunikacja.h"

class cServoA : public cSilnik
{
  private: //wskazniki LCD sa dziedziczone
    cKomunikacja* _pKomunikacja;
  
    double _dKPpart; //zwiększane kawałki od aktualnego punktu P do nowopodanego punktu P (w iteracjach)
    double _dKPtemp; //tymczasowy y,z,kp do obliczen (używane przy d_y_part, d_z_part, d_kp_part)

  public:
    //----------------------------------------------------------KONSTRUKTOR-------------------------------------------------------------//
    cServoA(int nPin, String sNazwaKata, int nKatMin, int nKatMax, cLCD_angle* pLCD_angle, cLCD_pos* pLCD_pos, cKomunikacja* pKomunikacja)
    {
      _nPin = nPin;
      _sNazwaKata = sNazwaKata;
      _nKatMin = nKatMin;
      _nKatMax = nKatMax;
      _pLCD_angle = pLCD_angle;
      _pLCD_pos = pLCD_pos;
      _nPredkosc = MOTOR_SPEED_NORMAL;
      _pKomunikacja = pKomunikacja;

      _dKPpart = 0;
      _dKPtemp = 0;
    }

    //------------------------------------------------------------METODY-------------------------------------------------------------//
    void ZwiekszKPpartTempem()                     { _dKPtemp += _dKPpart; }

    /*virtual*/ void UstawKatSerwisowo(String sKomendaRdzenia) 
    {
      int nKat = sKomendaRdzenia.substring(5).toInt();
      int nOgraniczenieKataDolne = 15;  //poza zakresem tym nie bardzo reagowało serwo
      int nOgraniczenieKataGorne = 165;

      if (nKat >= nOgraniczenieKataDolne && nKat <= nOgraniczenieKataDolne)
      {
        this->UstawKat(nKat);
        _pLCD_angle->PrintAngle(_sNazwaKata, this->getKat());
        Serial.print("kp = ");
        Serial.println(nKat);
        _pLCD_pos->PrintPos("x", -1); //ruszenie ręcznie kąta podstawy gubi wszystkie pozycje
        _pLCD_pos->PrintPos("y", -1);
        _pLCD_pos->PrintPos("z", -1);
      }
      else
      {
        Serial.print("Kat podstawy podany poza zakresem: <");
        Serial.print(nOgraniczenieKataDolne);
        Serial.print(",");
        Serial.print(nOgraniczenieKataDolne);
        Serial.print(">: ");
        Serial.println(nKat);
      }
    }

    String Trash()
    {
      _pKomunikacja->PrzygotujOdpowiedzNaRdzen("r_trash");
      this->UstawKat(175); //ustaw podstawe nad pundełkiem na zbite bierki...
      _pLCD_angle->PrintAngle("servoA", this->getKat() );
      _pLCD_pos->PrintPos("x", -1); //speed ustawiony na sztywno
      String sXYprzyResecie = "(170,240)"; //...i wyceluj tam na płaszczyźnie (y,z).
      return sXYprzyResecie;
    }

    //----------------------------------------------------METODY-DOSTĘPOWE-DO-PÓL----------------------------------------------------//
    double getKPpart() const                          { return _dKPpart; }
    double getKPtemp() const                          { return _dKPtemp; }

    void setKPpart(double dKPpart)                 { _dKPpart = dKPpart; }
    void setKPtemp(double dKPtemp)                 { _dKPtemp = dKPtemp; }
};

#endif
