//-------------------------------------------------------------INCLUDES-------------------------------------------------------------//
#pragma once

#include <VarSpeedServo.h> //biblioteka odpowiedzialna za powolny ruch serw (nowa funkcja write() )
#include <LiquidCrystal.h> //panele lcd
#include <math.h> //cosinusy, PI itd
#include "Arduino.h" //chyba to jest potrzebne do includów or smtg, ale chyba nie tu, tj. tu jest a utomatu
#include "silnik.h"
#include "lcd_pos.h"
#include "lcd_angle.h"
#include "komunikacja.h"
#include "serwis.h"
#include "szachownica.h"
#include "ramie.h"
#include "servo_a.h"
#include "servo_b.h"
#include "servo_c.h"
#include "servo_d.h"
#include "servo_e.h"
#include "servo_f.h"

cLCD_angle LCD_angle(32, 30, 28, 26, 24, 22, 16, 2);
cLCD_pos LCD_pos(33, 31, 29, 27, 25, 23, 20, 2);

cSilnik servoA(2, "servoA", "kp", 15, 175, &LCD_angle, &LCD_pos);
cSilnik servoB(3, "servoB", "alpha", 0, 180, &LCD_angle, &LCD_pos); // !!za duże wartości!!
cSilnik servoC(4, "servoC", "beta", 24, 157, &LCD_angle, &LCD_pos);
cSilnik servoD(5, "servoD", "fi", 45, 180, &LCD_angle, &LCD_pos);
cSilnik servoE(6, "servoE", "ks1", 82, 131, &LCD_angle, &LCD_pos); //nie potrzeba by działało dla szerszych wartości
cSilnik servoF(7, "servoF", "ks2", 200 - 131, 200 - 82, &LCD_angle, &LCD_pos); //2gie servo w szczekach jest obrocone(180 stopno) i przesuniete o 20 stopni w dzialaniu

cKomunikacja Komunikaty;
cSerwis Serwis(&Komunikaty, &servoB); //!!!!! wrzucone losowe serwo. przysiasc nad tym i dobrze sie zastanowic nad rozplanowaniem tej funkjci
cSzachownica Szachownica(&Komunikaty,  &LCD_pos);

cServoA ServoA_kp;
cServoA ServoB_alpha;
cServoA ServoC_beta(&Komunikaty);;
cServoA ServoD_fi;
cServoA ServoE_ks1;
cServoA ServoF_ks2;

cRamie Ramie(&Komunikaty, &LCD_angle, &servoE, &servoF);

String sCoreCommand;
//potrzebna (świeciło się ładnie na pomarańczowo i było użyte w jakimś przykładzie na niesie. na wszelki wypadek zostawiam to tutaj). sCoreCommand - jedna z ważniejszych zmiennych.
//zapisywane jest w niej to co przyjdzie po USB i program działa w opraciu o to co w niej jest. można zmienić tą nazwę bo w sumie głupia jest (użyta z przykładu z neta i tak zostało).
//reszta zmiennych to po prostu Stringi potrzebne przy konwersji z sCoreCommand na wartości liczbowe (i tyczą się one wartości y,z ważnych do liczenia punktu P na płaszczyźnie (y,z).
int nServoSpeed = 18; //predkosc dla serw podczas wstepnego ustawiania. wartość wykładnicza. im wyższe wartości tym szybciej chodzi (max działający = bodajże 127).
const int SERVO_SPEED = nServoSpeed; //podczas pracy ramienia prędkość ruchu serw jest zmieniana. by wracać to pierwotnej używamy tej stałej.
int nSpeedChanged = 0; // program co zakończenie ruchu ustawia sobie zdefiniowaną stałą prędkość ruchu ramienia. jeżeli ręcznie zmienimy tą prędkość funkcją serwisową "speed ="...
//...to będzie utrzymywał prędkość tej tutaj zmiennej. 0 na starcie by omijać tą zmienną jak nic nie ruszymy.
int nWspRuchu = 65; // ustawione na 65 było //współczynnik definiujący ilość pojedyńczych skoków/iteracji w ruchu ramienia (jego przenoszenia). wartość na wyczucie.
bool bPrzerwaniePetli = false; //przerywa pętle po wstepnym ustawieniu serw (by wykonało tylko jedną iterację, anie wszystkie z "nWspRuchu").

float fWektorOdchylek[] = { -11, -10.9, -10.8, -10.7, -10.6, -10.5, -10.4, -10.3, -10.2, -10.1, -10, -9.7, -9.4, -9.1, -8.8, -8.5, -8.2, -7.9, -7.6, -7.3, -7, -7.3, -7.6, -7.9, -8.2,
                            -8.5, -8.8, -9.1, -9.4, -9.7, -10, -9.5, -9, -8.5, -8, -7.5, -7, -6.5, -6, -5.5, -5, -5, -5, -5, -5, -5, -5, -5, -5, -5, -5, -4.9, -4.8, -4.7, -4.6, -4.5, -4.4, -4.3, -4.2, -4.1, -4,
                            -4, -4, -4, -4, -4, -4, -4, -4, -4, -4, -3.7, -3.4, -3.1, -2.8, -2.5, -2.2, -1.9, -1.6, -1.3, -1, -0.9, -0.8, -0.7, -0.6, -0.5, -0.4, -0.3, -0.2, -0.1, 0, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6,
                            0.7, 0.8, 0.9, 1, 1.2, 1.4, 1.6, 1.8, 2, 2.2, 2.4, 2.6, 2.8, 3, 3.2, 3.4, 3.6, 3.8, 4, 4.2, 4.4, 4.6, 4.8, 5, 5.2, 5.4, 5.6, 5.8, 6, 6.2, 6.4, 6.6, 6.8, 7, 7.2, 7.4, 7.6, 7.8, 8, 8.2,
                            8.4, 8.6, 8.8, 9, 9.1, 9.2, 9.3, 9.4, 9.5, 9.6, 9.7, 9.8, 9.9, 10, 10.3, 10.6, 10.9, 11.2, 11.5, 11.8, 12.1, 12.4, 12.7, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13.5, 14, 14.5, 15,
                            15.5, 16, 16.5, 17, 17.5, 18, 18.1, 18.2, 18.3, 18.4, 18.5, 18.6, 18.7, 18.8, 18.9, 19, 19.2, 19.4, 19.6, 19.8, 20, 20.2, 20.4, 20.6, 20.8, 21, 21.4, 21.8, 22.2, 22.6, 23, 23.4, 23.8,
                            24.2, 24.6, 25, 25.3, 25.6, 25.9, 26.2, 26.5, 26.8, 27.1, 27.4, 27.7, 28, 27.9, 27.8, 27.7, 27.6, 27.5, 27.4, 27.3, 27.2, 27.1, 27, 27.9, 28.8, 29.7, 30.6, 31.5, 32.4, 33.3, 34.2, 35.1,
                            36
                          }; //wektor poprawek wysokości (oś z). wyliczony doświadczalnie (nigdy poprawki nie są idealne). wektor koryguje wartości z zakresu od y = 100 (1st element of array) do 340 mm (last)


//-------------------------------------------------------------SETUP-------------------------------------------------------------//
void setup() //kod kóry wykona się raz
{
  servoA.Rozpocznij(); servoB.Rozpocznij(); servoC.Rozpocznij();
  servoD.Rozpocznij(); servoE.Rozpocznij(); servoF.Rozpocznij();

  servoA.UstawKat(90, MOTOR_SPEED_FAST);
  servoB.UstawKat(90, MOTOR_SPEED_FAST);
  servoC.UstawKat(90, MOTOR_SPEED_FAST);
  servoD.UstawKat(90 + n_fi_poprawka, MOTOR_SPEED_FAST);
  servoE.UstawKat(90, MOTOR_SPEED_FAST);
  servoF.UstawKat(200 - 90, MOTOR_SPEED_FAST);

  LCD_angle.PrintAngle("servoA", servoA.getKat()); LCD_angle.PrintAngle("servoB", servoB.getKat());
  LCD_angle.PrintAngle("servoC", servoC.getKat()); LCD_angle.PrintAngle("servoD", servoD.getKat());
  LCD_angle.PrintAngle("servoE", servoE.getKat()); LCD_angle.PrintAngle("servoF", servoF.getKat());

  pinMode(SERVO_POWER_PIN1, OUTPUT); //ustawianie tego pinu jako wyjście, tj. będziemy tu czymś sterować. pin włącza przekaźnikiem zasilanie na serva
  pinMode(SERVO_POWER_PIN2, OUTPUT);
  digitalWrite(SERVO_POWER_PIN1, HIGH); //przekaźnik działa na odwrót, zatem by był on na starcie wyłączony to dajemy na jego pin stan wysoki
  digitalWrite(SERVO_POWER_PIN2, HIGH);

  sCoreCommand = "(184,296)"; //ustawienie dla pierwszego uruchomienia łapy. dla tego punktu P(y,z) wszystkie ważne serwa powinny mieć ustawione 90 stopni po przejściu przez kod
  Serial.begin(9600); //Rozpocznij komunikację po usb z prędkośćią 9600 bitów/s
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
    if (sCoreCommand.substring(0, 5) == "reset") Reset(); //funckja serwisowa. ustawia serwa na pozycję startową
    else if (sCoreCommand.substring(0, 7) == "speed =") Serwis.ZmienPredkosc(sCoreCommand);
    else if (sCoreCommand.substring(0, 7) == "turn on") Serwis.WlaczSerwa();
    else if (sCoreCommand.substring(0, 8) == "turn off") Serwis.WylaczSerwa();
    else if (sCoreCommand.substring(0, 7) == "info on") Serwis.WlaczInfo();
    else if (sCoreCommand.substring(0, 8) == "info off") Serwis.WylaczInfo();
    else if (sCoreCommand.substring(0, 4) == "kp =") servoA.UstawKatSerwisowo(sCoreCommand.substring(0, 2), sCoreCommand.substring(5));
    else if (sCoreCommand.substring(0, 7) == "alpha =") servoB.UstawKatSerwisowo(sCoreCommand.substring(0, 5), sCoreCommand.substring(8));
    else if (sCoreCommand.substring(0, 6) == "beta =") servoC.UstawKatSerwisowo(sCoreCommand.substring(0, 4), sCoreCommand.substring(7));
    else if (sCoreCommand.substring(0, 4) == "fi =") servoD.UstawKatSerwisowo(sCoreCommand.substring(0, 2), sCoreCommand.substring(5));
    else if (sCoreCommand.substring(2, 6) == "open") Ramie.Open(); //otwieranie szczęk w celu odłożenia bierki na planszy, albo zejścia po nią
    else if (sCoreCommand.substring(2, 7) == "close") Ramie.Close(); //analogicznie do open
    else if (sCoreCommand.substring(2, 7) == "trash") sCoreCommand = ServoA_kp.Trash(); //ustaw się nad pojemnik ze zbitymi bierkiami. metoda trash ustawi na kp, a zwrot z f da y,z
    else if (sCoreCommand.substring(0, 4) == "ks =") Ramie.UstawKatSzczeki(sCoreCommand.substring(5).toInt() ); //funckja serwisowa. ustaw kąt silnika E i F (szczęk)

    //poniżej ogromny blok funkcji dla takich wartości sCoreCommand, które powodują ruch ramienia w wielu przejściach/iteracjach/mini-ruchach
    else if (sCoreCommand.substring(0, 1) == "(" || sCoreCommand.substring(2, 3) == "[" ||
             sCoreCommand.substring(2, 4) == "up" || sCoreCommand.substring(2, 6) == "down")
    {
      if (sCoreCommand.substring(0, 1) == "(" || sCoreCommand.substring(2, 3) == "[")
      {
        if (sCoreCommand.substring(0, 1) == "(") //funkcja serwisowa. warunek czyta wartość 'y', 'z' dla wartości sCoreCommand z formatu: '(yyy,zzz)' oraz '(yyy,zzz,A1)'.
        {
          Ramie.setY(sCoreCommand.substring(1, 4).toInt()); // y
          Ramie.setZ(sCoreCommand.substring(5, 8).toInt()); // z
        }
        if (sCoreCommand.substring(11, 12) == ")" || sCoreCommand.substring(5, 6) == "]") //wartośc z nawiasem okrągłym serwisowa, z kwadratowym służaca do obsługi gry.
          //warunek czyta wartość pola na planszy dla wartości sCoreCommand z formatu: '(yyy,zzz,A1)' lub '[a1]'.
        {
          Szachownica.OkreslLiterePola(sCoreCommand);
          Szachownica.OkreslCyfrePola(sCoreCommand);
          servoA.UstawKat(Szachownica.ObliczKatPodstawy(), MOTOR_SPEED_NORMAL);
          if (sCoreCommand.substring(2, 3) == "[")  Ramie.setY(Szachownica.Oblicz_y()); //dla  komendy z core z prośbą o ruch oblicz jeszcze długość y do liczenia kąta podstawy (składowa
          //'x' jest obliczona w obiekcie szachownicy, a wartość osi 'z' wynika tylko z funkcji up/down
        }
      }
      else
      {
        if (Ramie.getY() == 0) Ramie.setY(Ramie.getDlugoscB()); //jeżeli po odpaleniu ramienia nie była wprowadzona żadna wartość y, to ustaw domyślną. to jest ważne gdy po odpaleniu ramienia pierwszą rzeczą jaką...
        //...chcemy zrobić to podnieśc lub opóścić ramię, a nie ma jeszcze w programie wgranej żadnej wartości dla 'y'.
        if (sCoreCommand.substring(2, 4) == "up") Ramie.Up(); //jeżeli nie są przeprowadzane żadne ruchy po polach, to wpadamy do funkcji up/down. one też działają na ruch iteracyjnie
        else if (sCoreCommand.substring(2, 6) == "down") Ramie.Down();
      }

      ObliczPartyPrzemieszczenia(); //dla każdego normalnego podanego ruchu oblicz części ruchu do dodawania w pętli ruchu
      for (int i = 0; i < nWspRuchu; i++) //wykonuj cząstkowe ruchy "nWspRuchu" razy, aż ruch się wykona, lub do błędnych kątów serw. duży warunek.
      {
        DodajDoPunktuPCzescRuchuRamienia();
        if (i >= nWspRuchu - 1) WyrownanieTempowPoDodaniuOstatnichPartow(); //docelowy zadany punkt. jeśli gdzieś powstałyby minimalne błędy, to tu są naprawiane.
        //poniżej poprawka wysokosci - niech program myśli że jego zadane wartości są idealnie odwzorywane, a realnie obliczaj i ustawiaj kąty tak, by wyszło lepiej o skompensowany zmierzony błąd
        Ramie.setZtemp(Ramie.getZtemp() + fWektorOdchylek[nYtemp - 100]); LCD_pos.PrintPos("z", Ramie.getZtemp() ); //dodaj różnicę wysokości podstawy chwytaka i planszy
        Komunikaty.PokazInfo(INFO_ODLEGLOSC_Z_UPGR, (String)nZtemp); //poprawkę wysokości jako metoda ramienia
        OgraniczenieWysokosci();

        if (servoB.getKat() <= 180 && servoB.getKat() >= 0 && servoC.getKat() <= 157 && servoC.getKat() >= 24 && servoD.getKat() <= 180 && servoD.getKat() >= 45) UstawKatySerw(); //dopuszczalne kąty.
        else WartoscServaPozaDopuszczalnymZakresem(); //jeżeli kąt wyskoczył poza dopuszczalne wartości, to ustaw bezpieczne wartości, pokaż gdzie był błąd w zmiennych i wyjdź z pętli

        delay(15000 / (nServoSpeed * nWspRuchu)); //po każdej iteracji czekaj chwilę, aby serwa zdążyły powoli dojechać. współczynnik wyznaczony empirycznie (tj. na oko).
        if (bPrzerwaniePetli == true) PrzerwijPetle(); //wyjscie z petli for w szczegolnych wypadkach- error lub
      }

      PrewencyjnieDociagnijDoZadanegoPolozenia(); //wartości mimo iż są ok, przypisywane są jeszcze raz, bo możliwe że po wielu przejściach kąty będą się przesuwać o małe wartości

      if (nSpeedChanged == 0) nServoSpeed = SERVO_SPEED;  //predkosc serw podczas normalnej pracy (polecenia 'up' i 'down' zmieniają prędkość. tutaj trzeba to naprawiać)...
      else nServoSpeed = nSpeedChanged; //...jeżeli jednak prędkość była zmieniana ręcznie to będzie ona wracała zawsze do podanej ręcznie wartości.

      if (Ramie.getUpState() == true && Komunikaty.getSekwencjaRuchow()) //jeżeli łapa się podniosła podczas rozgrywania partii szachów, to podnieś łapę maxymalnie nad bierki...
      { //...poprzez danie maxymalnego kąta fi (wyprostowanie ramienia)
        servoD.UstawKat(179, MOTOR_SPEED_FAST, true); //parametr true powinien wykonać najpierw podniesienie zanim kod ruszy dalej
        delay(10); //nie wiem czemu to służy, ale używane tego przy wartości true na tutorialu, więc nie zaszkodzi
        LCD_angle.PrintAngle("servoD", servoD.getKat());
      }
      else if (Ramie.getDownState() == false) //po przeniesieniu się nad planszą ustaw sobie już kąt do zejścia po bierkę
      {
        servoD.UstawKat(270 - servoB.getKat() - servoC.getKat() + Ramie.getFiPoprawka(), MOTOR_SPEED_FAST);
        LCD_angle.PrintAngle("servoD", servoD.getKat());
      }

      if (Komunikaty.getSekwencjaRuchow() == true) //jeżeli mieliśmy do czynienia z ruchem generowanym z gry/ze strony...
      {
        if (Komunikaty.InfoDlaRdzenia() == "armUp2") //... i jeżeli był to ostatni ruch z sekwencji przemieszczania pionka...
        {
          servoA.UstawKat(110, static_cast<MOTOR_SPEED>(nServoSpeed)); LCD_angle.PrintAngle("servoA", servoA.getKat() );  //... to serwem podstawy ustaw się na środku planszy ...
          Komunikaty.setSekwencjaRuchow(false); //... i nie wykonuj już więcej funkcji związanym z grą na stronie WWW...
        }
        //... a na końcu wyślij do core informację o tym jaki żądany ruch z core'a został wykonany:
        // jeżeli żądanie ruchu zawierało pole nad które miała się łapa ruszyć, to do informacji zwrotnej dopisz też informację o tym jakie to było pole (zbędna ochrona)...
        if (Komunikaty.InfoDlaRdzenia().substring(2, 7) == "moved")
          Komunikaty.OdpowiedzNaRdzen(Komunikaty.InfoDlaRdzenia(), " " + Szachownica.getLiteraPola() + Szachownica.getCyfraPola() );
        else Komunikaty.OdpowiedzNaRdzen(Komunikaty.InfoDlaRdzenia(), ""); //...a dla innych ruchów wyślij samą odpowiedź o wykonanym ruchu.
        // !!w rdzeniu (core) mam stare ustawienia wg których każda wiadomość kończy się dolarem. zobaczyć czy to coś psuje (nie widać by coś się złego działo).
      }

      if (Ramie.getDownState == false && Komunikaty.InfoDlaRdzenia() != "r_trashed")  //jeżeli funkcja nie wykonywała ruchu typu down ani usuwania bierki
        ServoB_alpha.PodniesPrewencyjnie(); // to podnies prewecyjnie, aby nie wywracało bierek skrajnych

      /*czyszczenie zmiennych*/
      Ramie.setUpState(false); Ramie.setDownState(false); //jeżeli był wykonywny ruch typu up/down to zresetuj te zmienne, by w kolejnym przejściu arduino nie zareagowało na nie.
      sCoreCommand = ""; //po przejściu przez cały program wyczyść tą zmienną, by w kolejnym przejściu nie został uruchomiony któryś warunek.
    }
  }
}

