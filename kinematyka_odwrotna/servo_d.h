#ifndef servo_d_h
#define servo_d_h

#include "Arduino.h"
#include <VarSpeedServo.h>
#include <LiquidCrystal.h>
#include "silnik.h"
#include "lcd_angle.h"
#include "lcd_pos.h"
#include "komunikacja.h"

class cServoD : public cSilnik
{
  private: //wskazniki LCD sa dziedziczone
    cKomunikacja* _pKomunikacja;
    int _nFiPoprawka; //programowo zmienione fi, jako że mechanicznie bardzo ciężko jest to ustalić z taką dokładkością

  public:
    //----------------------------------------------------------KONSTRUKTOR----------------------------------------------------------//
    cServoD(int nPin, float fKatMin, float fKatMax, cLCD_angle* pLCD_angle, cKomunikacja* pKomunikacja)
    {
      _nPin = nPin;
      _fKatMin = fKatMin;
      _fKatMax = fKatMax;
      _pKomunikacja = pKomunikacja;
      _pLCD_angle = pLCD_angle;
      _nPredkosc = MOTOR_SPEED_NORMAL;
      _nFiPoprawka = -1;
      _pKomunikacja = pKomunikacja;
    }

    //------------------------------------------------------------METODY-------------------------------------------------------------//
    //servo fi minimum 45, bo by gniotło się o ramię przed nim

    void PrzygotujFi(bool bUp, bool bDown, bool bSekwencjaRuchow, double dAlpha, double dBeta)
    {
      if (bUp == true && bDown == true) //jeżeli łapa się podniosła podczas rozgrywania partii szachów, to podnieś łapę maxymalnie nad bierki...
      { //...poprzez danie maxymalnego kąta fi (wyprostowanie ramienia)
        this->write(179, MOTOR_SPEED_FAST, true); //parametr true powinien wykonać najpierw podniesienie zanim kod ruszy dalej
        delay(10); //nie wiem czemu to służy, ale używane tego przy wartości true na tutorialu, więc nie zaszkodzi
        _pLCD_angle->PrintAngle("servoD", this->Kat());
      }
      else if (bPozycjaDolna == false) //po przeniesieniu się nad planszą ustaw sobie już kąt do zejścia po bierkę
      {
        _fKat = 270 - dAlpha - dBeta;
        this->write(_fKat + _nFiPoprawka, MOTOR_SPEED_FAST, false);
        _pLCD_angle->PrintAngle("servoD", this->Kat() );
      }
      else Serial.println("error: PrzygotujFi");
    }
    void ObliczKatFi(bool bUp, bool bDown, String sKomendaRdzenia)
    {
      if (bUp == true || bDown == true || sKomendaRdzenia.substring(0, 1) == "(") //jeżeli ruch łapa idzie w dół/górę lub ruch odbywa się funkcją serwisową...
      { //... to licz kąt fi normalnie, tak by chwytak był zawsze prostopadły do planszy...
        _fKat = 270 - _dAlpha - _dBeta; //kąt fi docelowy
      }
      else _fKat = 179; //...a jeżeli nie to podnieś chwytak najwyżej jak się da, by przy przenoszeniu bierek nie chaczył o inne.

      if (_fKat == 179) _pKomunikacja->PokazInfo(INFO_KAT_FI, (String)_fKat);
      else _pKomunikacja->PokazInfo(INFO_KAT_FI, (String)(_fKat + _nFiPoprawka) );
    }

    void UstawFiDoKolejnegoRuchu(cRamie* pRamie, cServoB* pServoB, cServoC* pServoC)
    {
      if (pRamie->getUpState() == true && _pKomunikacja->getSekwencjaRuchow() ) //jeżeli łapa się podniosła podczas rozgrywania partii szachów, to podnieś łapę maxymalnie nad bierki...
      { //...poprzez danie maxymalnego kąta fi (wyprostowanie ramienia)
        this->UstawKat(179, MOTOR_SPEED_FAST, true); //poprzez parametr true ramie wykona najpierw podniesienie, zanim kod ruszy dalej
        delay(10); //nie wiem czemu to służy, ale używane tego przy wartości true na tutorialu, więc nie zaszkodzi
        _pLCD_angle->PrintAngle("ServoD_fi", _fKat); // ! dodac _nFiPoprawka ? (arcysczegol)
      }
      else if (pRamie->getDownState() == false) //po przeniesieniu się nad planszą ustaw sobie już kąt do zejścia po bierkę
      {
        this->UstawKat(270 - pServoB->getKat() - pServoC->getKat() + pRamie->getFiPoprawka(), MOTOR_SPEED_FAST);
        _pLCD_angle->PrintAngle("ServoD_fi", _fKat); // ! dodac _nFiPoprawka ? (arcysczegol)
      }
      else Serial.println("error: UstawFiDoKolejnegoRuchu");
    }

    //----------------------------------------------------METODY-DOSTĘPOWE-DO-PÓL----------------------------------------------------//
    int getFiPoprawka() const                  { return _nFiPoprawka; }
};

#endif
