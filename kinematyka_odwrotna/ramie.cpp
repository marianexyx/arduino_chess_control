#include "Arduino.h"
#include <VarSpeedServo.h>
#include "silnik.h"
#include "ramie.h"
#include "komunikacja.h"

cRamie::cRamie(cKomunikacja* pKomunikacja, cLCD_angle* pLCD_angle, cSilnik* pSilnikE, cSilnik* pSilnikF)
{
  _pKomunikacja = pKomunikacja;
  _pLCD_angle = pLCD_angle;
  _pSilnikE = pSilnikE;
  _pSilnikF = pSilnikF;
  _dA = 180;
  _dB = 184;
  _dC = 142;
  _dY = 0; //ważne było by wartość 'y' była ustawiona na starcie na 0, by któraśtam funkcja dalej w programie wiedziała że ma się wykonać tylko za pierwszym razem
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
  _dL = static_cast<double>(sqrt(ulZ0kwadrat + ulYkwadrat) ); //przekątna od servaB do punktu P(y,z)
}

void cRamie::Open()
{
  int nSzczekiOtwarte = 102;
  servoE.UstawKat(nSzczekiOtwarte, static_cast<MOTOR_SPEED>(n_servo_speed));
  _pLCD_angle->PrintAngle("servoE", servoE.getKat());
  servoF.UstawKat(200 - nSzczekiOtwarte, static_cast<MOTOR_SPEED>(n_servo_speed));
  _pLCD_angle->PrintAngle("servoF", servoF.getKat());
  _pKomunikacja->PokazInfo(INFO_OPEN, (String)nSzczekiOtwarte);
}

void cRamie::Close()
{
  int nSzczekiZamkniete = 83;
  servoE.UstawKat(nSzczekiZamkniete, static_cast<MOTOR_SPEED>(n_servo_speed));
  _pLCD_angle->PrintAngle("servoE", servoE.getKat());
  servoF.UstawKat(200 - nSzczekiZamkniete, static_cast<MOTOR_SPEED>(n_servo_speed));
  _pLCD_angle->PrintAngle("servoF", servoF.getKat());
  _pKomunikacja->PokazInfo(INFO_CLOSE, (String)nSzczekiZamkniete);
}

void cRamie::Up()
{
  _bUp = true;
  _dZ = 230; //jedna z 2 możliwych 'z' możliwych docelowo do osiągnięcia w programie (inne są zbędne)
  n_servo_speed = MOTOR_SPEED_SLOW; //predkosc serw podczas podnoszenia od planszy //!!!!!!!!!!!!!!!!!!!!1
  _pKomunikacja->PokazInfo(INFO_UP, (String)_dZ);
}

void cRamie::Down()
{
  _bDown = true;
  _dZ = 158; //jw. w 'up'
  n_servo_speed = MOTOR_SPEED_SLOW; //predkosc serw podczas opadania ku planszy
  _pKomunikacja->PokazInfo(INFO_DOWN, (String)_dZ);
}

void UstawKatSzczeki(int nKatSzczek)
{
  if (nKatSzczek >= 82 && nKatSzczek <= 131) //nie potrzeba by działało dla szerszych wartości
  {
    _nKatSzczeki = nKatSzczek;
    _pSilnikE->UstawKat(_nKatSzczeki, MOTOR_SPEED_NORMAL);  _pLCD_angle->PrintAngle("servoE", _pSilnikE->getKat() );
    //drugie serwo w szczękach uastawia sobie kąt na odwrót, by współgrać z tym pierwszym. współczynnik zdobyty doświadczalnie
    _pSilnikF->UstawKat(200 - _nKatSzczeki, MOTOR_SPEED_NORMAL);  _pLCD_angle->PrintAngle("servoF", _pSilnikF->getKat() );
    Serial.print("kat szczeki = ");
    Serial.println(_nKatSzczeki);
  }
  else Serial.println("katSzczeki podany poza zakresem <82,131>");
}

