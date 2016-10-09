//-------------------------------------------------------------INCLUDES-------------------------------------------------------------//
#pragma once

#include <VarSpeedServo.h> //biblioteka odpowiedzialna za powolny ruch serw (nowa funkcja write() )
#include <LiquidCrystal.h> //panele lcd
#include <math.h> //cosinusy, PI itd
#include <stdlib.h> // dla dtostrf
#include "Arduino.h" //chyba to jest potrzebne do includów or smtg, ale chyba nie tu, tj. tu jest a utomatu

#include "silnik.h"
#include "lcd_pos.h"
#include "lcd_angle.h"
#include "komunikacja.h"
#include "szachownica.h"
//#include "ramie.h"
#include "serwis.h"
#include "servo_a.h"
#include "servo_b.h"
#include "servo_c.h"
#include "servo_d.h"
#include "servo_ef.h"

cLCD_angle LCD_angle(32, 30, 28, 26, 24, 22, 16, 2);
cLCD_pos LCD_pos(33, 31, 29, 27, 25, 23, 20, 2);

cKomunikacja Komunikaty;

cServoA ServoA_kp(2, "kp", 15, 175, &LCD_angle, &LCD_pos, &Komunikaty);
cServoB ServoB_alpha(3, "alpha", 0, 180, &LCD_angle, &LCD_pos/*, &Komunikaty*/); // !!za duże wartości!!
cServoC ServoC_beta(4, "beta", 24, 157, &LCD_angle, &LCD_pos/*, &Komunikaty*/);
cServoD ServoD_fi(5, "fi", 45, 180, &LCD_angle, &LCD_pos, &Komunikaty);
cServoEF ServoE_ks1(6, "ks1", 82, 131, &LCD_angle, &LCD_pos/*, &Komunikaty*/); //nie potrzeba by działało dla szerszych wartości
cServoEF ServoF_ks2(7, "ks2", 200-131, 200-82, &LCD_angle, &LCD_pos/*i, &Komunikaty*/); //2gie servo w szczekach jest obrocone(180 stopno) i przesuniete o 20 stopni w dzialaniu

cSzachownica Szachownica(&Komunikaty,  &LCD_pos);
cRamie Ramie(&Komunikaty, &LCD_angle, &ServoE_ks1, &ServoF_ks2);
cSerwis Serwis(&Komunikaty, &Ramie);

enum CORE_COMMAND { CC_RESET, CC_UP, CC_DOWN, CC_SPEED, CC_TURNON, CC_TURNOFF, CC_INFOON, CC_INFOOFF, CC_KP, CC_ALPHA, CC_BETA, CC_FI, CC_KS, CC_OPEN, CC_CLOSE, CC_TRASH, CC_YZ, CC_XYZ, CC_XY, CC_ELSE };
/*extern*/ CORE_COMMAND KomendaRdzenia;
String sCoreCommand;// sCoreCommand - jedna z ważniejszych zmiennych. zapisywane jest w niej to co przyjdzie po USB i program działa w opraciu o to co w niej jest.
int nWspRuchu = 65; // ustawione na 65 było //współczynnik definiujący ilość pojedyńczych skoków/iteracji w ruchu ramienia (jego przenoszenia). wartość na wyczucie.
bool bPrzerwaniePetli = false; //przerywa pętle po wstepnym ustawieniu serw (by wykonało tylko jedną iterację, anie wszystkie z "nWspRuchu").

//-------------------------------------------------------------SETUP-------------------------------------------------------------//
void setup() //kod kóry wykona się raz
{
  Serial.begin(9600);

  ServoA_kp.Rozpocznij(); ServoB_alpha.Rozpocznij(); ServoC_beta.Rozpocznij();
  ServoD_fi.Rozpocznij(); ServoE_ks1.Rozpocznij(); ServoF_ks2.Rozpocznij();

  ServoA_kp.UstawKat(90, MOTOR_SPEED_FAST); LCD_angle.PrintAngle("ServoA_kp", ServoA_kp.getKat());
  ServoB_alpha.UstawKat(90, MOTOR_SPEED_FAST); LCD_angle.PrintAngle("ServoB_alpha", ServoB_alpha.getKat());
  ServoC_beta.UstawKat(90, MOTOR_SPEED_FAST); LCD_angle.PrintAngle("ServoC_beta", ServoC_beta.getKat());
  ServoD_fi.UstawKat(90 + ServoD_fi.getFiPoprawka(), MOTOR_SPEED_FAST); LCD_angle.PrintAngle("ServoD_fi", ServoD_fi.getKat());
  ServoE_ks1.UstawKat(90, MOTOR_SPEED_FAST); LCD_angle.PrintAngle("ServoE_ks1", ServoE_ks1.getKat());
  ServoF_ks2.UstawKat(200 - 90, MOTOR_SPEED_FAST); LCD_angle.PrintAngle("ServoF_ks2", ServoF_ks2.getKat());

  pinMode(SERVO_POWER_PIN1, OUTPUT); //ustawianie tego pinu jako wyjście, tj. będziemy tu czymś sterować. pin włącza przekaźnikiem zasilanie na serva
  pinMode(SERVO_POWER_PIN2, OUTPUT);
  digitalWrite(SERVO_POWER_PIN1, HIGH); //przekaźnik działa na odwrót, zatem by był on na starcie wyłączony to dajemy na jego pin stan wysoki
  digitalWrite(SERVO_POWER_PIN2, HIGH);

  sCoreCommand = "(184,296)"; //ustawienie dla pierwszego uruchomienia łapy. dla tego punktu P(y,z) wszystkie ważne serwa powinny mieć ustawione 90 stopni po przejściu przez kod
  Serial.println("Podano automatycznie pierwszy ruch: (184,296)");
}

//-------------------------------------------------------------MAIN-------------------------------------------------------------//
void loop() //wieczna główna pętla programu
{
  while (Serial.available()) //gdy pojawi się na porcie jakaś wiadomość...
  {
    sCoreCommand = Serial.readString(); //... zapisz ją w tej zmiennej.
    Komunikaty.PrzygotujOdpowiedzNaRdzen(sCoreCommand);
  }
  if (sCoreCommand != "")
  {
    SprawdzTypPolecenia(sCoreCommand);

    switch (KomendaRdzenia)
    {
      case CC_SPEED: Serwis.ZmienPredkosc(sCoreCommand); break;
      case CC_TURNON: Serwis.WlaczSerwa(); break;
      case CC_TURNOFF: Serwis.WylaczSerwa(); break;
      case CC_INFOON: Serwis.WlaczInfo(); break;
      case CC_INFOOFF: Serwis.WylaczInfo(); break;
      case CC_KP: ServoA_kp.UstawKatSerwisowo(sCoreCommand);  break;
      case CC_ALPHA: ServoB_alpha.UstawKatSerwisowo(sCoreCommand); break;
      case CC_BETA: ServoC_beta.UstawKatSerwisowo(sCoreCommand); break;
      case CC_FI: ServoD_fi.UstawKatSerwisowo(sCoreCommand); break;
      case CC_KS: Ramie.UstawKatSzczeki(sCoreCommand.substring(5).toInt()); break;
      case CC_OPEN: Ramie.Open(); break;
      case CC_CLOSE: Ramie.Close();  break;
      case CC_TRASH: sCoreCommand = ServoA_kp.Trash(); break; //ustaw się nad pojemnik ze zbitymi bierkiami. metoda trash ustawi na kp, a zwrot z f da y,z
      case CC_RESET:
        Reset();
        WykonajRuchRamieniem();
        break;
      case CC_UP: //ruch po osi 'y' (w górę)
        Ramie.Up();
        WykonajRuchRamieniem();
        break;
      case CC_DOWN: //ruch po osi 'y' (w dół)
        Ramie.Down();
        WykonajRuchRamieniem();
        break;
      case CC_YZ: // (yyy,zzz) - ruch (serwisowy) na 2 plaszczyznach: y,z
        Ramie.setY(sCoreCommand.substring(1,4).toInt()); // ustaw y
        Ramie.setZ(sCoreCommand.substring(5,8).toInt()); // ustaw z
        WykonajRuchRamieniem();
        break;
      case CC_XYZ: // (yyy,zzz,a1) - ruch (serwisowy) na wszystkich 3 plaszczyznach
        Ramie.setY(sCoreCommand.substring(1,4).toInt()); // ustaw y
        Ramie.setZ(sCoreCommand.substring(5,8).toInt()); // ustaw z
        Szachownica.OkreslLiterePola(sCoreCommand);
        Szachownica.OkreslCyfrePola(sCoreCommand);
        Szachownica.ObliczKatPodstawy(); // na podstawie litery i cyfry pola szachownicy
        WykonajRuchRamieniem();
        break;
      case CC_XY: // np. [a1] - ruch (z rdzenia) na 2 plaszczyznach: x,y
        Szachownica.OkreslLiterePola(sCoreCommand);
        Szachownica.OkreslCyfrePola(sCoreCommand);
        Szachownica.ObliczKatPodstawy(); //na podstawie litery i cyfry pola szachownicy
        Ramie.setY(Szachownica.Oblicz_y());
        WykonajRuchRamieniem();
        break;
      default:  Serial.println(sCoreCommand); break;
    }
    sCoreCommand = "";
  }
}
