#ifndef servo_c_h
#define servo_c_h

#include "Arduino.h"
#include <VarSpeedServo.h>
#include <LiquidCrystal.h>
#include "silnik.h"
#include "lcd_angle.h"
#include "lcd_pos.h"
//#include "komunikacja.h"
#include <math.h>

class cServoC : public cSilnik
{
  private: //wskazniki LCD sa dziedziczone

  public:
    //----------------------------------------------------------KONSTRUKTOR----------------------------------------------------------//
    cServoC(int nPin, String sNazwaKata, int nKatMin, int nKatMax, cLCD_angle* pLCD_angle, cLCD_pos* pLCD_pos/*, cKomunikacja* pKomunikacja*/)
    {
      _nPin = nPin;
      _sNazwaKata = sNazwaKata;
      _nKatMin = nKatMin;
      _nKatMax = nKatMax;
      _pLCD_angle = pLCD_angle;
      _pLCD_pos = pLCD_pos;
      _nPredkosc = MOTOR_SPEED_NORMAL;
      //_pKomunikacja = pKomunikacja;
    }

    //------------------------------------------------------------METODY-------------------------------------------------------------//
    //servo beta dziala w przedziale od 24 do 157 stopni.
    float ObliczKatBeta(double dL, double dA, double dB, cKomunikacja* pKomunikacja)
    {
      double dBetaRad = acos((dA * dA + dB * dB - dL * dL) / (2 * dA * dB)); //obliczanie bety w radianach
      _dKat = (180 / PI) * dBetaRad; //kąt beta docelowy.
      if (_dKat >= 157) _dKat = 156; //!!servo beta ma słaby zakres!!!
      //pKomunikacja->PokazInfo(_dKat, INFO_KAT_BETA);
      return _dKat;
    }

    /*virtual*/ void UstawKatSerwisowo(String sKomendaRdzenia) //funckja serwisowa - !! to wrzucic do ktorejstam wirtualnej metody kazdego serva
    {
      int nKat = sKomendaRdzenia.substring(7).toInt();;
      int nOgraniczenieKataDolne = 24;  //nienajlepsze serwo powoduje wąski przedział działających wartości
      int nOgraniczenieKataGorne = 157;

      if (nKat >= nOgraniczenieKataDolne && nKat <= nOgraniczenieKataDolne)
      {
        this->UstawKat(nKat);
        _pLCD_angle->PrintAngle(_sNazwaKata, this->getKat() );
        Serial.print("beta = ");
        Serial.println(nKat);
        _pLCD_pos->PrintPos("y", -1); //ruszenie ręcznie kąta podstawy gubi wszystkie pozycje
        _pLCD_pos->PrintPos("z", -1);
      }
      else
      {
        Serial.print("Kat beta podany poza zakresem: <");
        Serial.print(nOgraniczenieKataDolne);
        Serial.print(",");
        Serial.print(nOgraniczenieKataDolne);
        Serial.print(">: ");
        Serial.println(nKat);
      }
    }
};

#endif
