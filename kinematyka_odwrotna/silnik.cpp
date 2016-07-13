#include "Arduino.h"
#include "silnik.h"
#include <VarSpeedServo.h>
#include <LiquidCrystal.h>

cSilnik::cSilnik(int nPin, String sNazwaServa, String sNazwaKata, float fKatMin, float fKatMax, cLCD_angle* pLCD_angle, cLCD_pos* pLCD_pos)
{
  _nPin = nPin;
  _sNazwaServa = sNazwaServa;
  _sNazwaKata = sNazwaKata;
  _fKatMin = fKatMin;
  _fKatMax = fKatMax;
  _pLCD_angle = pLCD_angle; //możliwość odwoływania się do innej klasy w klasie
  _pLCD_pos = pLCD_pos;
}

void cSilnik::rozpocznij()
{
  _Servo.attach(_nPin); //podpinanie serwomechanizmów do pinów. piny muszą być PWN
}

void cSilnik::ustaw_kat(float fKat)
//predkosc automatycznie na NORMAL
//bez podania 3go parametru serva zawsze czekaja az wykona sie ich ruch zanim przejda do kolejnej funckji
{
  if (fKat >= _fKatMin || fKat <= _fKatMax) //prawidlowy zakres
  {
    _fKat = fKat;
    _Servo.write((int)_fKat, MOTOR_SPEED_NORMAL, false);
    
    _pLCD_angle->PrintAngle(_sNazwaServa, this->Kat());
    _pLCD_pos->PrintPos("y", -1); 
    _pLCD_pos->PrintPos("z", -1); 

    /*if funckja_serwisowa
      Serial.print("beta = ");
      Serial.println(beta);*/
  }
  else //poza zakresem dzialania //! sprobowac kiedys sprintf'em
  {
    Serial.print("Kat "); Serial.print(_sNazwaKata); Serial.print(" podany poza zakresem <");
    Serial.print(_fKatMin); Serial.print(","); Serial.print(_fKatMax); Serial.println(">");
  }
}

void cSilnik::ustaw_kat(float fKat, MOTOR_SPEED predkoscSilnika)
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
}

void cSilnik::ustaw_kat(float fKat, MOTOR_SPEED predkoscSilnika, bool bCzekajNaKoniecRuchu)
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
}

void cSilnik::podniesPrewencyjnie(bool bOpuszczono, String sCoreAnswer, int nAlpha) // warunek aby nie wywracało bierek skrajnych
{
  if (bOpuszczono == false && sCoreAnswer != "r_trashed") //jeżeli funkcja nie wykonywała ruchu typu down ani usuwania bierki
  {
    nAlpha += 8;
    _Servo.write(nAlpha, MOTOR_SPEED_NORMAL, true); //!!!! normalnie predkosc = 6
  }
}

void cSilnik::przygotujFi(bool bPozycjaGorna, bool bPozycjaDolna, bool bSekwencjaRuchow, double dFi, int nFiPoprawka, double dAlpha, double dBeta) //servo D tu wrzucamy
{
  if (bPozycjaGorna == true && bSekwencjaRuchow == true) //jeżeli łapa się podniosła podczas rozgrywania partii szachów, to podnieś łapę maxymalnie nad bierki...
  { //...poprzez danie maxymalnego kąta fi (wyprostowanie ramienia)
    _Servo.write(179, MOTOR_SPEED_FAST, true); //parametr true powinien wykonać najpierw podniesienie zanim kod ruszy dalej
    delay(10); //nie wiem czemu to służy, ale używane tego przy wartości true na tutorialu, więc nie zaszkodzi
    _pLCD_angle->PrintAngle("servoD", this->Kat());
  }
  else if (bPozycjaDolna == false) //po przeniesieniu się nad planszą ustaw sobie już kąt do zejścia po bierkę
  {
    dFi = 270 - dAlpha - dBeta;
    _Servo.write(dFi + nFiPoprawka, MOTOR_SPEED_FAST, false);
    _pLCD_angle->PrintAngle("servoD", this->Kat());
  }

  //!!!!!!!!!!!!!!!!!!
  /*if (b_up == true && b_sekwencja_ruchow == true) //jeżeli łapa się podniosła podczas rozgrywania partii szachów, to podnieś łapę maxymalnie nad bierki...
    { //...poprzez danie maxymalnego kąta fi (wyprostowanie ramienia)
    servoD.write(179, 70, true); //parametr true powinien wykonać najpierw podniesienie zanim kod ruszy dalej
    delay(10); //nie wiem czemu to służy, ale używane tego przy wartości true na tutorialu, więc nie zaszkodzi
    PrintAngleToLCD("servoD");
    }
    else if (b_down == false) //po przeniesieniu się nad planszą ustaw sobie już kąt do zejścia po bierkę
    {
    fi = 270 - alpha - beta;
    servoD.write(fi + n_fi_poprawka, 70, false);
    PrintAngleToLCD("servoD");
    }*/
}

void cSilnik::otworz()
{
  //!!!!!!!!!!!!!!!!!!
  /*n_katSzczeki = 102;
    servoE.write(n_katSzczeki, n_servo_speed, false); PrintAngleToLCD("servoE");
    servoF.write(n_wsp_ks2 - n_katSzczeki, n_servo_speed, false); PrintAngleToLCD("servoF");
    if (b_show_info == true)
    {
    Serial.print("opened. katSzczeki = ");
    Serial.println(n_katSzczeki);
    }
    //poniżej warunki gdy jest wykonywany oficjalny ruch bierki podczas rozgrywki
    if (coreCommand.substring(0, 7) == "n_open1" && b_sekwencja_ruchow == true) AnswerToCore("", "n_opened1");
    else if (coreCommand.substring(0, 7) == "n_open2" && b_sekwencja_ruchow == true) AnswerToCore("", "n_opened2");
    else if (coreCommand.substring(0, 7) == "r_open1" && b_sekwencja_ruchow == true) AnswerToCore("", "r_opened1");
    else if (coreCommand.substring(0, 7) == "r_open2" && b_sekwencja_ruchow == true) AnswerToCore("", "r_opened2");
    else if (coreCommand.substring(0, 7) == "c_open1" && b_sekwencja_ruchow == true) AnswerToCore("", "c_opened1");
    else if (coreCommand.substring(0, 7) == "c_open2" && b_sekwencja_ruchow == true) AnswerToCore("", "c_opened2");
    else if (coreCommand.substring(0, 7) == "c_open3" && b_sekwencja_ruchow == true) AnswerToCore("", "c_opened3");
    coreCommand = "";*/
}

void cSilnik::zamknij()
{
  //!!!!!!!!!!!!!!!!!!
  /*n_katSzczeki = 83;
    servoE.write(n_katSzczeki, n_servo_speed, false); PrintAngleToLCD("servoE");
    servoF.write(n_wsp_ks2 - n_katSzczeki, n_servo_speed, false); PrintAngleToLCD("servoF");
    if (b_show_info == true)
    {
    Serial.print("closed. katSzczeki = ");
    Serial.println(n_katSzczeki);
    }
    if (coreCommand.substring(0, 8) == "n_close1" && b_sekwencja_ruchow == true) AnswerToCore("", "n_closed1");
    else if (coreCommand.substring(0, 7) == "r_close" && b_sekwencja_ruchow == true) AnswerToCore("", "r_closed");
    else if (coreCommand.substring(0, 8) == "c_closeK" && b_sekwencja_ruchow == true) AnswerToCore("", "c_closedK");
    else if (coreCommand.substring(0, 8) == "c_closeR" && b_sekwencja_ruchow == true) AnswerToCore("", "c_closedR");
    coreCommand = "";*/
}

/*void cSilnik::smietnik() // to sie odnosi do ramienia
  {
  //ustaw podstawe nad pundełkiem na zbite bierki...
  servoA.ustaw_kat(175); LCD_angle.PrintAngle("servoA", servoA.Kat()); LCD_pos.PrintPos("x", -1); //PrintAngleToLCD("servoA"); PrintPosToLCD("x", -1); //servoA.write(n_katPodstawa, 18, false); //speed ustawiony na sztywno
  coreCommand = "(170,240)"; //...i wyceluj tam na płaszczyźnie (y,z).
  Str_move_case = "r_trashed";
  //!!!!!!!!!!!!!!!!!!
  n_katPodstawa = 175; //ustaw podstawe nad pundełkiem na zbite bierki...
    servoA.write(n_katPodstawa, 18, false); PrintAngleToLCD("servoA"); PrintPosToLCD("x", -1); //speed ustawiony na sztywno
    coreCommand = "(170,240)"; //...i wyceluj tam na płaszczyźnie (y,z).
    Str_move_case = "r_trashed";
  }*/

