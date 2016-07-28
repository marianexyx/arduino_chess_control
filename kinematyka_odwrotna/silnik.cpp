#include "Arduino.h"
#include "silnik.h"
#include <VarSpeedServo.h>
#include <LiquidCrystal.h>
#include <math.h>

cSilnik::cSilnik(int nPin, String sNazwaKata, float fKatMin, float fKatMax, cLCD_angle* pLCD_angle, cLCD_pos* pLCD_pos, cRamie* pRamie, cKomunikacja* pKomunikacja)
{
  _nPin = nPin;
  _sNazwaKata = sNazwaKata;
  _fKatMin = fKatMin;
  _fKatMax = fKatMax;
  _nPredkosc = MOTOR_SPEED_NORMAL;
  _pLCD_angle = pLCD_angle;
  _pLCD_pos = pLCD_pos;
  _pRamie = pRamie;
  _pKomunikacja = pKomunikacja;
}

void cSilnik::Rozpocznij()
{
  _Servo.attach(_nPin); //podpinanie serwomechanizmów do pinów. piny muszą być PWN
}

void cSilnik::UstawKat(float fKat, MOTOR_SPEED PredkoscSilnika, bool bCzekajNaKoniecRuchu)
{
  if (fKat >= _fKatMin || fKat <= _fKatMax) //prawidlowy zakres
  {
    _fKat = fKat;
    _Servo.write((int)_fKat, PredkoscSilnika, bCzekajNaKoniecRuchu);

    _pLCD_angle->PrintAngle(_sNazwaServa, this->Kat());
    _pLCD_pos->PrintPos("y", -1);
    _pLCD_pos->PrintPos("z", -1);

    _pKomunikacja->PokazInfo(INFO_KAT_BETA, (String)_fKat);
  }
  else //poza zakresem dzialania 
  {
    Serial.print("Kat "); Serial.print(_sNazwaKata); Serial.print(" podany poza zakresem <"); 
    Serial.print(_fKatMin); Serial.print(","); Serial.print(_fKatMax); Serial.print(">: ");  Serial.println(_fKat);
  }
}

void cSilnik::UstawKatSerwisowo(String sKomendaRdzenia) //funckja serwisowa - !! to wrzucic do ktorejstam wirtualnej metody kazdego serva
{
  String sKat, sNazwakata;
  int nOgraniczenieKataDolne, nOgraniczenieKataGorne;

  if (sKomendaRdzenia.substring(0, 2) == "kp")
  {
    sNazwakata = sKomendaRdzenia.substring(0, 2);
    sKat = sKomendaRdzenia.substring(5);
    nOgraniczenieKataDolne = 15;  //poza zakresem tym nie bardzo reagowało serwo
    nOgraniczenieKataGorne = 165;
  }
  else if (sKomendaRdzenia.substring(0, 5) == "alpha")
  {
    sNazwakata = sKomendaRdzenia.substring(0, 5);
    sKat = sKomendaRdzenia.substring(8);
    nOgraniczenieKataDolne = 0;
    nOgraniczenieKataGorne = 180;
  }
  else if (sKomendaRdzenia.substring(0, 4) == "beta")
  {
    sNazwakata = sKomendaRdzenia.substring(0, 4);
    sKat = sKomendaRdzenia.substring(7);
    nOgraniczenieKataDolne = 24;  //nienajlepsze serwo powoduje wąski przedział działających wartości
    nOgraniczenieKataGorne = 157;
  }
  else if (sKomendaRdzenia.substring(0, 2) == "fi")
  {
    sNazwakata = sKomendaRdzenia.substring(0, 2);
    sKat = sKomendaRdzenia.substring(5);
    nOgraniczenieKataDolne = 15;
    nOgraniczenieKataGorne = 175;
  }

  int nKatPodstawa = sKat.toInt();
  if (nKatPodstawa >= nOgraniczenieKataDolne && nKatPodstawa <= nOgraniczenieKataDolne)
  {
    this->UstawKat(nKatPodstawa);
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

