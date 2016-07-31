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
    cServoD(int nPin, String sNazwaKata, int nKatMin, int nKatMax, cLCD_angle* pLCD_angle, cLCD_pos* pLCD_pos, cKomunikacja* pKomunikacja)
    {
      _nPin = nPin;
      _sNazwaKata = sNazwaKata;
      _nKatMin = nKatMin;
      _nKatMax = nKatMax;
      _pKomunikacja = pKomunikacja;
      _pLCD_angle = pLCD_angle;
      _nPredkosc = MOTOR_SPEED_NORMAL;
      _nFiPoprawka = -1;
      _pKomunikacja = pKomunikacja;
    }

    //------------------------------------------------------------METODY-------------------------------------------------------------//
    //servo fi minimum 45, bo by gniotło się o ramię przed nim

    /*void PrzygotujFi(bool bUp, bool bDown, bool bSekwencjaRuchow, double dAlpha, double dBeta) // po zakonczeniu ruchu ustaw ramie by szybciej zejsc w dol, lub nie wywrócić bierek
      {
      if (bUp == true && bDown == true) //jeżeli łapa się podniosła podczas rozgrywania partii szachów, to podnieś łapę maxymalnie nad bierki...
      { //...poprzez podanie maxymalnego kąta fi (wyprostowanie ramienia)
        _Servo.write(179, MOTOR_SPEED_FAST, true); //parametr true powinien wykonać najpierw podniesienie zanim kod ruszy dalej
        delay(10); //nie wiem czemu to służy, ale używane tego przy wartości true na tutorialu, więc nie zaszkodzi
        _pLCD_angle->PrintAngle("servoD", this->getKat());
      }
      else if (bDown == false) //po przeniesieniu się nad planszą ustaw sobie już kąt do zejścia po bierkę
      {
        _fKat = 270 - dAlpha - dBeta;
        _Servo.write(_fKat + _nFiPoprawka, MOTOR_SPEED_FAST, false);
        _pLCD_angle->PrintAngle("servoD", this->getKat() );
      }
      else Serial.println("error: PrzygotujFi");
      }*/

    /*void ObliczKatFi(bool bUp, bool bDown, String sKomendaRdzenia, double dAlpha, double dBeta)
    {
      if (bUp == true || bDown == true || sKomendaRdzenia.substring(0, 1) == "(") //jeżeli ruch łapa idzie w dół/górę lub ruch odbywa się funkcją serwisową...
      { //... to licz kąt fi normalnie, tak by chwytak był zawsze prostopadły do planszy...
        _fKat = 270 - dAlpha - dBeta; //kąt fi docelowy
      }
      else _fKat = 179; //...a jeżeli nie to podnieś chwytak najwyżej jak się da, by przy przenoszeniu bierek nie chaczył o inne.

      if (_fKat == 179) _pKomunikacja->PokazInfo(INFO_KAT_FI, (String)_fKat);
      else _pKomunikacja->PokazInfo(INFO_KAT_FI, (String)(_fKat + _nFiPoprawka) );
    }*/

    void ObliczKatFi(cRamie* pRamie, cServoB* pServoB, cServoC* pServoC) //ustawianie fi w dół planszy, lub horyzontalnie do niej, wykonywane po skonczeniu ktoregos ruchu
    {
      if (pRamie->getUpState() == true && _pKomunikacja->getSekwencjaRuchow() ) //jeżeli łapa się podniosła podczas rozgrywania partii szachów, to podnieś łapę maxymalnie nad bierki...
      { //...poprzez danie maxymalnego kąta fi (wyprostowanie ramienia)
        this->UstawKat(179, MOTOR_SPEED_FAST, true); //poprzez parametr true ramie wykona najpierw podniesienie, zanim kod ruszy dalej
        delay(10); //nie wiem czemu to służy, ale używane tego przy wartości true na tutorialu, więc nie zaszkodzi
        _pLCD_angle->PrintAngle("ServoD_fi", 179); // ! dodac _nFiPoprawka ? (arcysczegol)
        _pKomunikacja->PokazInfo(INFO_KAT_FI, "179");
      }
      else if (pRamie->getDownState() == false || _pKomunikacja->getSekwencjaRuchow() == false) //jezeli ramie nie schodzilo w dół i nie mamy doczynienia zk omenda serwisowa
      {
        int nKatFi = 270 - pServoB->getKat() - pServoC->getKat() + _nFiPoprawka;
        this->UstawKat(nKatFi, MOTOR_SPEED_FAST);
        _pLCD_angle->PrintAngle("ServoD_fi", nKatFi); // ! dodac _nFiPoprawka ? (arcysczegol)
        _pKomunikacja->PokazInfo(INFO_KAT_FI, (String)nKatFi);
      }
      else Serial.println("error: UstawFiDoKolejnegoRuchu");
    }

    /*virtual*/ void UstawKatSerwisowo(String sKomendaRdzenia) //funckja serwisowa - !! to wrzucic do ktorejstam wirtualnej metody kazdego serva
    {
      int nKat = sKomendaRdzenia.substring(5).toInt();
      int nOgraniczenieKataDolne = 15;
      int nOgraniczenieKataGorne = 175;

      if (nKat >= nOgraniczenieKataDolne && nKat <= nOgraniczenieKataDolne)
      {
        this->UstawKat(nKat);
        _pLCD_angle->PrintAngle(_sNazwaKata, this->getKat());
        Serial.print("fi = ");
        Serial.println(nKat);
      }
      else
      {
        Serial.print("Kat fi podany poza zakresem: <");
        Serial.print(nOgraniczenieKataDolne);
        Serial.print(",");
        Serial.print(nOgraniczenieKataDolne);
        Serial.print(">: ");
        Serial.println(nKat);
      }
    }
    //----------------------------------------------------METODY-DOSTĘPOWE-DO-PÓL----------------------------------------------------//
    int getFiPoprawka() const                 { return _nFiPoprawka; }
};

#endif
