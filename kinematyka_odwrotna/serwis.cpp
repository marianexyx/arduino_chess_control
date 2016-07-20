#include "Arduino.h"
#include "serwis.h"

cSerwis::cSerwis(cKomunikacja* pKomunikacja,  cSilnik* pSilnik) 
{
  _pKomunikacja = pKomunikacja;
  _pSilnik = pSilnik;
}

void cSerwis::ZmienPredkosc(String sKomendaRdzenia) //funckja serwisowa. ustawia prędkość przemieszczania się ramienia (bez up/down)
{
  MOTOR_SPEED predkoscSilnika = static_cast<MOTOR_SPEED>(sKomendaRdzenia.substring(8).toInt());
  _pSilnik->setPredkosc(predkoscSilnika);
  Serial.print("servo speed = "); Serial.println(predkoscSilnika);
}
void cSerwis::WlaczSerwa() //daj zasilanie na serwa...
{
  digitalWrite(SERVO_POWER_PIN1, LOW);
  digitalWrite(SERVO_POWER_PIN2, LOW);
  _pKomunikacja->OdpowiedzNaRdzen("turn on");
}
void cSerwis::WylaczSerwa() //!!!wyłączenie serw. jeżeli nie jest łapa dobrze oparta, to runie ona o podłoże. wprowadzić funkcję auto podpierania przed wyłączeniem.
{
  digitalWrite(SERVO_POWER_PIN1, HIGH);
  digitalWrite(SERVO_POWER_PIN2, HIGH);
  _pKomunikacja->OdpowiedzNaRdzen("turn off");
}
void cSerwis::WlaczInfo() //pokazuj stan wszystkiego co się dzieje na procku (wyrzucaj info na port). włączamy do prac serwisowych. włączenie gry automatycznie to wyłączy.
{
  _pKomunikacja->PokazujInfo(true);
  _pKomunikacja->OdpowiedzNaRdzen("info on");
}
void cSerwis::WylaczInfo() //wyłącz info- analogicznie do tego powyżej. arduino jest po starcie na "info off"
{
  _pKomunikacja->PokazujInfo(false);
  _pKomunikacja->OdpowiedzNaRdzen("info off");
}
