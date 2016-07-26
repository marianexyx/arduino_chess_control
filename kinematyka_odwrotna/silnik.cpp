#include "Arduino.h"
#include "silnik.h"
#include <VarSpeedServo.h>
#include <LiquidCrystal.h>
#include <math.h>

cSilnik::cSilnik(int nPin, float fKatMin, float fKatMax, cLCD_angle* pLCD_angle, cLCD_pos* pLCD_pos)
{
  _nPin = nPin;
  _fKatMin = fKatMin;
  _fKatMax = fKatMax;
  _pLCD_angle = pLCD_angle; 
  _pLCD_pos = pLCD_pos;
}

void cSilnik::Rozpocznij()
{
  _Servo.attach(_nPin); //podpinanie serwomechanizmów do pinów. piny muszą być PWN
}

void cSilnik::UstawKat(float fKat, MOTOR_SPEED predkoscSilnika)
//bez podania 3go parametru serva zawsze czekaja az wykona sie ich ruch zanim przejda do kolejnej funckji
{
  if (fKat >= _fKatMin || fKat <= _fKatMax) //prawidlowy zakres
  {
    _fKat = fKat;
    _Servo.write((int)_fKat, predkoscSilnika, false);

    _pLCD_angle->PrintAngle(_sNazwaServa, this->Kat());
    _pLCD_pos->PrintPos("y", -1);
    _pLCD_pos->PrintPos("z", -1);
  }
  else //poza zakresem dzialania //! sprobowac kiedys sprintf'em
  {
    Serial.print("Kat "); Serial.print(_sNazwaKata); Serial.print(" podany poza zakresem <");
    Serial.print(_fKatMin); Serial.print(","); Serial.print(_fKatMax); Serial.println(">");
  }  
    /*if funckja_serwisowa
      Serial.print("beta = ");
      Serial.println(beta);*/
}

void cSilnik::UstawKat(float fKat, MOTOR_SPEED predkoscSilnika, bool bCzekajNaKoniecRuchu)
{
  if (fKat >= _fKatMin || fKat <= _fKatMax) //prawidlowy zakres
  {
    _fKat = fKat;
    _Servo.write((int)_fKat, predkoscSilnika, bCzekajNaKoniecRuchu);

    _pLCD_angle->PrintAngle(_sNazwaServa, this->Kat());
    _pLCD_pos->PrintPos("y", -1);
    _pLCD_pos->PrintPos("z", -1);
  }
  else //poza zakresem dzialania //! sprobowac kiedys sprintf'em
  {
    Serial.print("Kat "); Serial.print(_sNazwaKata); Serial.print(" podany poza zakresem <");
    Serial.print(_fKatMin); Serial.print(","); Serial.print(_fKatMax); Serial.println(">");
  }
    /*if funckja_serwisowa
      Serial.print("beta = ");
      Serial.println(beta);*/
}

void cSilnik::UstawKatSerwisowo(String sNazwaKata, String sKat) //funckja serwisowa
{
  int nOgraniczenieKataDolne, nOgraniczenieKataGorne;
  if (sNazwaKata == "alpha") {
    nOgraniczenieKataDolne = 0;
    nOgraniczenieKataGorne = 180;
  }
  else if (sNazwaKata == "beta") {
    nOgraniczenieKataDolne = 24;  //nienajlepsze serwo powoduje wąski przedział działających wartości
    nOgraniczenieKataGorne = 157;
  }
  else if (sNazwaKata == "fi") {
    nOgraniczenieKataDolne = 15;
    nOgraniczenieKataGorne = 175;
  }
  else if (sNazwaKata == "kp") {
    nOgraniczenieKataDolne = 15;  //poza zakresem tym nie bardzo reagowało serwo
    nOgraniczenieKataGorne = 165;
  }

  int nKatPodstawa = sKat.toInt();
  if (nKatPodstawa >= nOgraniczenieKataDolne && nKatPodstawa <= nOgraniczenieKataDolne)
  {
    this->UstawKat(nKatPodstawa, MOTOR_SPEED_NORMAL);
    _pLCD_angle->PrintAngle(_sNazwaServa, this->Kat());
    Serial.print(sNazwaKata);
    Serial.print(" = ");
    Serial.println(sKat);
    if (sNazwaKata != "fi")
    {
      if (sNazwaKata == "kp") _pLCD_pos->PrintPos("x", -1); //ruszenie ręcznie kąta podstawy gubi wszystkie pozycje
      _pLCD_pos->PrintPos("y", -1);
      _pLCD_pos->PrintPos("z", -1);
    }
    else Serial.print("Brak poprawki fi");
  }
  else
  {
    Serial.print(sNazwaKata);
    Serial.print(" podany poza zakresem: <");
    Serial.print(nOgraniczenieKataDolne);
    Serial.print(",");
    Serial.print(nOgraniczenieKataDolne);
    Serial.print(">");
  }
}

