#include "Arduino.h"
#include "serwis.h"

cSerwis::cSerwis(cKomunikacja* pKomunikacja, cRamie* pRamie)
{
  _pKomunikacja = pKomunikacja;
  _pRamie = pRamie;

  _nPredkoscSerwisowa = 0;
}

void cSerwis::ZmienPredkosc(String sKomendaRdzenia) //funckja serwisowa. ustawia prędkość przemieszczania się ramienia (bez up/down).
//w ramieniu istneije predkosc do aktualnego wykonywania ruchu, a w serwisie predkosc serwisowa do ktorej ramie bedzie wracac podczas czesciowych zmian predkosci w ruchach takich jak np. up/down
{
  _nPredkoscSerwisowa = sKomendaRdzenia.substring(8).toInt();
  _pRamie->setPredkosc(_nPredkoscSerwisowa);
  Serial.print("servo speed = "); Serial.println(sKomendaRdzenia.substring(8));
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

