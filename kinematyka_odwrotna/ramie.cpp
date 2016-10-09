#include "Arduino.h"
#include <VarSpeedServo.h>
#include "silnik.h"
#include "ramie.h"
#include "komunikacja.h"

double cRamie::_dL = 0; //pole statyczne

cRamie::cRamie(cKomunikacja* pKomunikacja, cLCD_angle* pLCD_angle, cSilnik* pSilnikE, cSilnik* pSilnikF)
:_fWektorOdchylek { -11, -10.9, -10.8, -10.7, -10.6, -10.5, -10.4, -10.3, -10.2, -10.1, -10, -9.7, -9.4, -9.1, -8.8, -8.5, -8.2, -7.9, -7.6, -7.3, -7, -7.3, -7.6, -7.9, -8.2,
                            -8.5, -8.8, -9.1, -9.4, -9.7, -10, -9.5, -9, -8.5, -8, -7.5, -7, -6.5, -6, -5.5, -5, -5, -5, -5, -5, -5, -5, -5, -5, -5, -5, -4.9, -4.8, -4.7, -4.6, -4.5, -4.4, -4.3, -4.2, -4.1, -4,
                            -4, -4, -4, -4, -4, -4, -4, -4, -4, -4, -3.7, -3.4, -3.1, -2.8, -2.5, -2.2, -1.9, -1.6, -1.3, -1, -0.9, -0.8, -0.7, -0.6, -0.5, -0.4, -0.3, -0.2, -0.1, 0, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6,
                            0.7, 0.8, 0.9, 1, 1.2, 1.4, 1.6, 1.8, 2, 2.2, 2.4, 2.6, 2.8, 3, 3.2, 3.4, 3.6, 3.8, 4, 4.2, 4.4, 4.6, 4.8, 5, 5.2, 5.4, 5.6, 5.8, 6, 6.2, 6.4, 6.6, 6.8, 7, 7.2, 7.4, 7.6, 7.8, 8, 8.2,
                            8.4, 8.6, 8.8, 9, 9.1, 9.2, 9.3, 9.4, 9.5, 9.6, 9.7, 9.8, 9.9, 10, 10.3, 10.6, 10.9, 11.2, 11.5, 11.8, 12.1, 12.4, 12.7, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13.5, 14, 14.5, 15,
                            15.5, 16, 16.5, 17, 17.5, 18, 18.1, 18.2, 18.3, 18.4, 18.5, 18.6, 18.7, 18.8, 18.9, 19, 19.2, 19.4, 19.6, 19.8, 20, 20.2, 20.4, 20.6, 20.8, 21, 21.4, 21.8, 22.2, 22.6, 23, 23.4, 23.8,
                            24.2, 24.6, 25, 25.3, 25.6, 25.9, 26.2, 26.5, 26.8, 27.1, 27.4, 27.7, 28, 27.9, 27.8, 27.7, 27.6, 27.5, 27.4, 27.3, 27.2, 27.1, 27, 27.9, 28.8, 29.7, 30.6, 31.5, 32.4, 33.3, 34.2, 35.1,
                            36
                          } //wektor poprawek wysokości (oś z). wyliczony doświadczalnie (nigdy poprawki nie są idealne). wektor koryguje wartości z zakresu //!! na pewno 241 to rozmiar tej tablicy?
//od y = 100 (1st element of array) do 340 mm (last), dlatego zawsze podczas zwracania odejmujemy wliczbę 100*/;
{
  _pKomunikacja = pKomunikacja;
  _pLCD_angle = pLCD_angle;
  _pSilnikE = pSilnikE;
  _pSilnikF = pSilnikF;
  
  _dA = 180;
  _dB = 184;
  _dC = 142;
  _dY = _dB; //jeżeli po odpaleniu ramienia nie była wprowadzona żadna wartość y, to ustaw domyślną. to jest ważne gdy po odpaleniu ramienia pierwszą rzeczą jaką...
  //...chcemy zrobić to podnieśc lub opóścić ramię, a nie ma jeszcze w programie wgranej żadnej wartości dla 'y'.
  _bUp = false;
  _bDown = false;
  _nZ1 = 116;
  _nKatSzczeki = 90;
  _dYpart = 0;
  _dZpart = 0;
  _dYtemp = 0;
  _dZtemp = 0;
}

void cRamie::ObliczPrzekatnaRamieniaL()
{
  unsigned long ulYkwadrat, ulZ0kwadrat; //druga potega y i z0 (wartości inaczej się nie mieszczą w obliczeniach- wypadają poza zasięgi zmiennych, dlatego są liczone osobno)
  unsigned long ulZ0; //odległość punktu P(y,z) od wysokośći na której jest servoB w pionie (wartość obliczana dopiero jak podane jest "z" w "P").

  ulYkwadrat = (unsigned long)_dYtemp * (unsigned long)_dYtemp;
  ulZ0 = (unsigned long)_dZtemp - _nZ1; //odległość punktu P(y,z) od wysokośći na której jest servoB w pionie- założenie, że punkt nigdy nie jest poniżej serva (nigdy nie będzie)
  ulZ0kwadrat = ulZ0 * ulZ0;
  cRamie::_dL = static_cast<double>(sqrt(ulZ0kwadrat + ulYkwadrat) ); //przekątna od servaB do punktu P(y,z)
}

void cRamie::Open() 
{
  int nSzczekiOtwarte = 102;
  _pSilnikE->UstawKat(nSzczekiOtwarte); _pLCD_angle->PrintAngle("servoE", _pSilnikE->getKat() );
  _pSilnikF->UstawKat(200 - nSzczekiOtwarte); _pLCD_angle->PrintAngle("servoF", _pSilnikF->getKat() );
  _pKomunikacja->PokazInfo((double)nSzczekiOtwarte, INFO_OPEN);
}

void cRamie::Close() 
{
  int nSzczekiZamkniete = 83;
  _pSilnikE->UstawKat(nSzczekiZamkniete); _pLCD_angle->PrintAngle("servoE", _pSilnikE->getKat() );
  _pSilnikF->UstawKat(200 - nSzczekiZamkniete); _pLCD_angle->PrintAngle("servoF", _pSilnikF->getKat() );
  _pKomunikacja->PokazInfo((double)nSzczekiZamkniete, INFO_CLOSE);
}

void cRamie::Up()
{
  _bUp = true;
  _dZ = 230; //jedna z 2 możliwych 'z' możliwych docelowo do osiągnięcia w programie (inne są zbędne)
  this->setPredkosc(MOTOR_SPEED_SLOW); //predkosc serw podczas podnoszenia od planszy
  _pKomunikacja->PokazInfo(_dZ, INFO_UP);
}

void cRamie::Down()
{
  _bDown = true;
  _dZ = 158; //jw. w 'up'
  this->setPredkosc(MOTOR_SPEED_SLOW); //predkosc serw podczas opadania ku planszy
  _pKomunikacja->PokazInfo(_dZ, INFO_DOWN);
}

void cRamie::UstawKatSzczeki(int nKatSzczek)
{
  if (nKatSzczek >= 82 && nKatSzczek <= 131) //nie potrzeba by działało dla szerszych wartości
  {
    _nKatSzczeki = nKatSzczek;
    _pSilnikE->UstawKat(_nKatSzczeki);  _pLCD_angle->PrintAngle("servoE", _pSilnikE->getKat() );
    //drugie serwo w szczękach uastawia sobie kąt na odwrót, by współgrać z tym pierwszym. współczynnik zdobyty doświadczalnie
    _pSilnikF->UstawKat(200 - _nKatSzczeki);  _pLCD_angle->PrintAngle("servoF", _pSilnikF->getKat() );
    Serial.print("kat szczeki = ");
    Serial.println(_nKatSzczeki);
  }
  else Serial.println("katSzczeki podany poza zakresem <82,131>");
}

