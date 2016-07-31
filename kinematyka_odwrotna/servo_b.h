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
    cServoB(int nPin, String sNazwaKata, int nKatMin, int nKatMax, cLCD_angle* pLCD_angle, cLCD_pos* pLCD_pos, cKomunikacja* pKomunikacja)
    {
      _nPin = nPin;
      _sNazwaKata = sNazwaKata;
      _nKatMin = nKatMin;
      _nKatMax = nKatMax;
      _pLCD_angle = pLCD_angle;
      _nPredkosc = MOTOR_SPEED_NORMAL;
      _pKomunikacja = pKomunikacja;
    }

    //------------------------------------------------------------METODY-------------------------------------------------------------//
    float ObliczKatAlfa(double dL, double dA, double dB, double dYtemp)
    {
      //pre_alpha 1 i 2 pomocnicze przy liczeniu kątów serw (problem z zasięgiem zmiennych)
      double dPreAlpha1Rad = (dL * dL + dA * dA - dB * dB) / (2 * dL * dA); //obliczanie alpha1 w radianach bez acos
      double dPreAlpha2Rad = dYtemp / dL; //obliczanie alpha2 w radianach bez acos
      double dAlphaRad = acos(dPreAlpha1Rad) + acos(dPreAlpha2Rad); //cały kąt alpha w radianach
      double dAlpha = (180 / PI) * dAlphaRad; //docelowy kąt alpha
      _fKat = dAlpha;

      _pKomunikacja->PokazInfo(INFO_KAT_ALPHA, (String)_fKat);

      return _fKat;
    }

    /*virtual*/ void UstawKatSerwisowo(String sKomendaRdzenia) //funckja serwisowa
    {
      int nKat = sKomendaRdzenia.substring(8).toInt();
      int nOgraniczenieKataDolne = 0;
      int nOgraniczenieKataGorne = 180;

      if (nKat >= nOgraniczenieKataDolne && nKat <= nOgraniczenieKataDolne)
      {
        this->UstawKat(nKat);
        _pLCD_angle->PrintAngle(_sNazwaKata, this->getKat());
        Serial.print("Kat alpha = ");
        Serial.println(nKat);
        _pLCD_pos->PrintPos("y", -1);  //ruszenie ręcznie kąta podstawy gubi wszystkie pozycje
        _pLCD_pos->PrintPos("z", -1);
      }
      else
      {
        Serial.print("Kat alpha podany poza zakresem: <");
        Serial.print(nOgraniczenieKataDolne);
        Serial.print(",");
        Serial.print(nOgraniczenieKataDolne);
        Serial.print(">: ");
        Serial.println(nKat);
      }
    }

    void PodniesPrewencyjnie()  //prewencyjne odsunięcie od planszy
    {
      _Servo.write(_fKat + 8, MOTOR_SPEED_NORMAL, true); //!!!! normalnie predkosc = 6
      _pLCD_angle->PrintAngle("servoB", _fKat);
    }
};

#endif
