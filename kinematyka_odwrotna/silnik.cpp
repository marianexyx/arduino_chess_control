#include "Arduino.h"
#include "silnik.h"
#include <VarSpeedServo.h>
#include <LiquidCrystal.h>
#include <math.h>

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

void cSilnik::Rozpocznij()
{
  _Servo.attach(_nPin); //podpinanie serwomechanizmów do pinów. piny muszą być PWN
}

void cSilnik::UstawKat(float fKat)
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
}

void cSilnik::PodniesPrewencyjnie(bool bOpuszczono, String sCoreAnswer, int nAlpha) // warunek aby nie wywracało bierek skrajnych
{
  if (bOpuszczono == false && sCoreAnswer != "r_trashed") //jeżeli funkcja nie wykonywała ruchu typu down ani usuwania bierki
  {
    nAlpha += 8;
    _Servo.write(nAlpha, MOTOR_SPEED_NORMAL, true); //!!!! normalnie predkosc = 6
  }
}

void cSilnik::PrzygotujFi(bool bPozycjaGorna, bool bPozycjaDolna, bool bSekwencjaRuchow, double dFi, int nFiPoprawka, double dAlpha, double dBeta) //servo D tu wrzucamy
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

double a = 180; double b = 184; double c = 142; //dlugosci ramion [mm]. dlugosc C mierzona do podstawy chwytaka. reszta to odległości pomiędzy orczykami serw

int ObliczKatAlfa(double d_a, double d_b)
{
  //pre_alpha 1 i 2 pomocnicze przy liczeniu kątów serw (problem z zasięgiem zmiennych)
  double pre_alpha1_rad = (L * L + a * a - b * b) / (2 * L * a); //obliczanie alpha1 w radianach bez acos
  double pre_alpha2_rad = n_y_temp / L; //obliczanie alpha2 w radianach bez acos
  double alpha_rad = acos(pre_alpha1_rad) + acos(pre_alpha2_rad); //cały kąt alpha w radianach
  double alpha = (180 / PI) * alpha_rad; //docelowy kąt alpha

  Komunikaty.PokazInfo(INFO_KAT_ALPHA, (String)alpha);

  return alpha; //ucięcie przecinka z double
}
int ObliczKatBeta()
{
  double beta_rad = acos((a * a + b * b - L * L) / (2 * a * b)); //obliczanie bety w radianach
  double beta = (180 / PI) * beta_rad; //kąt beta docelowy.

  if (beta >= 157) beta = 156; //!!servo beta ma słaby zakres!!! 

  Komunikaty.PokazInfo(INFO_KAT_BETA, (String)beta);
}
int ObliczKatFi()
{
  if (b_up == true || b_down == true || sCoreCommand.substring(0, 1) == "(") //jeżeli ruch łapa idzie w dół/górę lub ruch odbywa się funkcją serwisową...
  { //... to licz kąt fi normalnie, tak by chwytak był zawsze prostopadły do planszy...
    fi = 270 - alpha - beta; kąt fi docelowy
  }
  else fi = 179; //...a jeżeli nie to podnieś chwytak najwyżej jak się da, by przy przenoszeniu bierek nie chaczył o inne.

  if (fi == 179) Komunikaty.PokazInfo(INFO_KAT_FI, (String)fi);
  else Komunikaty.PokazInfo(INFO_KAT_FI, (String)(fi + n_fi_poprawka));
}

