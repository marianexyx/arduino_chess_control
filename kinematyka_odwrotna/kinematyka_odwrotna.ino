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
/* #include "ramie.h"*/

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

double fi1, fi2, fi3, fi_rad; //skomplikowane liczenie kąta fi w kilku krokach
int n_fi_poprawka = -1;
float f_katPodstawa; //kąt podstawy- serwo podstawy. jako float
int nKatPodstawa = 90; int nKatSzczeki = 90; //kąty podstawy i szczęk jako integery. na starcie zdefiniowane na 90 stopni (środek zakresu pracy serwa)
double alpha = 90, beta = 90, fi = 90, fi_temp = 90, fi_last = 90; // kąty serw przy ładowaniu programu. jw.
int n_y, n_z; // wymagany punkt P(y,z) jako integery. najważniejsze wartości w programie
unsigned long y = 0, z; //skladowe punktu P(y,z) jako ogromne liczby. najważniejsze wartości w programie. ważne było by wartość 'y' była ustawiona na starcie na 0.
//uwaga: wartośći y, z (czy n_y, n_z) są tak naprawdę współczynnikami pozycji punktu P, a nie realnymi wartościami tego punktu w milimetrach w przestrzeni rzeczywistej (wartości
//liczone od podstawy ramienia). nie sprawdziłem tych wartości przy liczeniu kinematyki odwrotnej i odbiegają one o jakieś 10% od tego co się da obliczyć linijką. dlatego później w
//programie występują zmienne typu: d_kp_y, d_kp_x, d_b, d_c (oraz cały blok zmiennych potrzebnych do liczenia ich) na podstawie których liczony jest kąt podstawy i jest komparacja
//położenia pól planszy szachownicy w stosunku do punktu P(y,z) liczonego na współczynnikach.
double d_y_part = 0, d_z_part = 0, d_kp_part = 0; //zwiększane kawałki od aktualnego punktu P do nowopodanego punktu P (w iteracjach)
double d_y_temp = 0, d_z_temp = 0, d_kp_temp = 0; //tymczasowy y,z,kp do obliczen (używane przy d_y_part, d_z_part, d_kp_part)
unsigned long n_y_temp, n_z_temp, n_kp_temp; //integery tempów do obliczeń (używane przy d_y_part, d_z_part)
String sCoreCommand, Str_y, Str_z, Str_katSzczeki, Str_katPodstawa, Str_fi, Str_alpha, Str_beta, Str_servo_speed; //, readString; <- nie wiem czemu myślałem że zmienna readString jest
//potrzebna (świeciło się ładnie na pomarańczowo i było użyte w jakimś przykładzie na niesie. na wszelki wypadek zostawiam to tutaj). sCoreCommand - jedna z ważniejszych zmiennych.
//zapisywane jest w niej to co przyjdzie po USB i program działa w opraciu o to co w niej jest. można zmienić tą nazwę bo w sumie głupia jest (użyta z przykładu z neta i tak zostało).
//reszta zmiennych to po prostu Stringi potrzebne przy konwersji z sCoreCommand na wartości liczbowe (i tyczą się one wartości y,z ważnych do liczenia punktu P na płaszczyźnie (y,z).
bool b_up = false; bool b_down = false; //zmienne informują czy w danych ruchu ramienia było ono podnoszone, czy opuszczane (ważne dla prostowania serwa kąta fi, by nic nie wywrócić).
int n_servo_speed = 18; //predkosc dla serw podczas wstepnego ustawiania. wartość wykładnicza. im wyższe wartości tym szybciej chodzi (max działający = bodajże 127).
const int SERVO_SPEED = n_servo_speed; //podczas pracy ramienia prędkość ruchu serw jest zmieniana. by wracać to pierwotnej używamy tej stałej.
int n_speed_changed = 0; // program co zakończenie ruchu ustawia sobie zdefiniowaną stałą prędkość ruchu ramienia. jeżeli ręcznie zmienimy tą prędkość funkcją serwisową "speed ="...
//...to będzie utrzymywał prędkość tej tutaj zmiennej. 0 na starcie by omijać tą zmienną jak nic nie ruszymy.
int n_wsp_ruchu = 65; // ustawione na 65 było //współczynnik definiujący ilość pojedyńczych skoków/iteracji w ruchu ramienia (jego przenoszenia). wartość na wyczucie.
bool b_przerywanie_petli = false; //przerywa pętle po wstepnym ustawieniu serw (by wykonało tylko jedną iterację, anie wszystkie z "n_wsp_ruchu").

float f_wektor_odchylek[] = { -11, -10.9, -10.8, -10.7, -10.6, -10.5, -10.4, -10.3, -10.2, -10.1, -10, -9.7, -9.4, -9.1, -8.8, -8.5, -8.2, -7.9, -7.6, -7.3, -7, -7.3, -7.6, -7.9, -8.2,
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

  LCD_angle.PrintAngle("servoA", servoA.Kat()); LCD_angle.PrintAngle("servoB", servoB.Kat());
  LCD_angle.PrintAngle("servoC", servoC.Kat()); LCD_angle.PrintAngle("servoD", servoD.Kat());
  LCD_angle.PrintAngle("servoE", servoE.Kat()); LCD_angle.PrintAngle("servoF", servoF.Kat());

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

  //w powyższym warunku głównie była zmieniana wartość zmiennej Komunikaty.InfoDlaRdzenia(). tutaj jest wykonywana reszta/większa część ruchów, gdzie np. wartość up2 wpada do warunku wyżej i tu.
  if (sCoreCommand.substring(0, 5) == "reset") //funckja serwisowa. ustawia serwa na pozycję startową
  {
    Komunikaty.OdpowiedzNaRdzen("reset");
    y = b; z = z1 + a; LCD_pos.PrintPos("y", y); LCD_pos.PrintPos("z", z); //ustawienia wynikajace z katow = 90
    sCoreCommand = "(184,296)"; //to samo co wyzej
    nKatSzczeki = 90; nKatPodstawa = 90;
    servoA.UstawKat(90, MOTOR_SPEED_NORMAL); LCD_angle.PrintAngle("servoA", servoA.Kat()); LCD_pos.PrintPos("x", -1);
    servoE.UstawKat(90, MOTOR_SPEED_NORMAL); LCD_angle.PrintAngle("servoE", servoE.Kat());
    servoF.UstawKat(200 - 90, MOTOR_SPEED_NORMAL); LCD_angle.PrintAngle("servoF", servoF.Kat());
  }
  else if (sCoreCommand.substring(0, 7) == "speed =") Serwis.ZmienPredkosc(sCoreCommand);
  else if (sCoreCommand.substring(0, 7) == "turn on") Serwis.WlaczSerwa();
  else if (sCoreCommand.substring(0, 8) == "turn off") Serwis.WylaczSerwa();
  else if (sCoreCommand.substring(0, 7) == "info on") Serwis.WlaczInfo();
  else if (sCoreCommand.substring(0, 8) == "info off") Serwis.WylaczInfo();
  else if (sCoreCommand.substring(0, 4) == "kp =") servoA.UstawKatSerwisowo(sCoreCommand.substring(0, 2), sCoreCommand.substring(5));
  else if (sCoreCommand.substring(0, 7) == "alpha =") servoB.UstawKatSerwisowo(sCoreCommand.substring(0, 5), sCoreCommand.substring(8));
  else if (sCoreCommand.substring(0, 6) == "beta =") servoC.UstawKatSerwisowo(sCoreCommand.substring(0, 4), sCoreCommand.substring(7));
  else if (sCoreCommand.substring(0, 4) == "fi =") servoD.UstawKatSerwisowo(sCoreCommand.substring(0, 2), sCoreCommand.substring(5));
  else if (sCoreCommand.substring(0, 4) == "ks =") //funckja serwisowa. ustaw kąt silnika E i F (szczęk)
  {
    Str_katSzczeki = sCoreCommand.substring(5);
    nKatSzczeki = Str_katSzczeki.toInt();
    if (nKatSzczeki >= 82 && nKatSzczeki <= 131) //nie potrzeba by działało dla szerszych wartości
    {
      servoE.UstawKat(nKatSzczeki, MOTOR_SPEED_NORMAL);  LCD_angle.PrintAngle("servoE", servoE.Kat());
      //drugie serwo w szczękach uastawia sobie kąt na odwrót, by współgrać z tym pierwszym. współczynnik zdobyty doświadczalnie
      servoF.UstawKat(200 - nKatSzczeki, MOTOR_SPEED_NORMAL);  LCD_angle.PrintAngle("servoF", servoF.Kat());
      Serial.print("kat szczeki = "); Serial.println(nKatSzczeki);
    }
    else Serial.println("katSzczeki podany poza zakresem <82,131>");
    sCoreCommand = "";
  }
  else if (sCoreCommand.substring(2, 6) == "open") Open(); //otwieranie szczęk w celu odłożenia bierki na planszy, albo zejścia po nią
  else if (sCoreCommand.substring(2, 7) == "close") Close(); //analogicznie do open

  //poniżej ogromny blok funkcji dla takich wartości sCoreCommand, które powodują ruch ramienia w wielu przejściach/iteracjach/mini-ruchach
  else if (sCoreCommand.substring(0, 1) == "(" || sCoreCommand.substring(2, 3) == "[" ||
           sCoreCommand.substring(2, 4) == "up" || sCoreCommand.substring(2, 6) == "down")
  {
    if (sCoreCommand.substring(0, 1) == "(" || sCoreCommand.substring(2, 3) == "[")
    {
      if (sCoreCommand.substring(0, 1) == "(") //funkcja serwisowa. warunek czyta wartość 'y', 'z' dla wartości sCoreCommand z formatu: '(yyy,zzz)' oraz '(yyy,zzz,A1)'.
      {
        Str_y = sCoreCommand.substring(1, 4); // y
        Str_z = sCoreCommand.substring(5, 8); // z
        n_y = Str_y.toInt(); n_z = Str_z.toInt(); //zmień odczytany string na int...
        y = n_y; z = n_z; //...a teraz zmień odczyt na unsigned long
        if (z < 150) z = 150;
      }
      if (sCoreCommand.substring(11, 12) == ")" || sCoreCommand.substring(5, 6) == "]") //wartośc z nawiasem okrągłym serwisowa, z kwadratowym służaca do obsługi gry.
        //warunek czyta wartość pola na planszy dla wartości sCoreCommand z formatu: '(yyy,zzz,A1)' lub '[a1]'.
      {
        Szachownica.OkreslLiterePola(sCoreCommand);
        Szachownica.OkreslCyfrePola(sCoreCommand);
        nKatPodstawa = Szachownica.ObliczKatPodstawy();   
        if (sCoreCommand.substring(2, 3) == "[")  y = Szachownica.Oblicz_y(); //dla  komendy z core z prośbą o ruch oblicz jeszcze długość y do liczenia kąta podstawy (składowa 
        //'x' jest obliczona w obiekcie szachownicy, a wartość osi 'z' wynika tylko z funkcji up/down
      }
    }
    else
    {
      if (y == 0) y = b; //jeżeli po odpaleniu ramienia nie była wprowadzona żadna wartość y, to ustaw domyślną. to jest ważne gdy po odpaleniu ramienia pierwszą rzeczą jaką...
      //...chcemy zrobić to podnieśc lub opóścić ramię, a nie ma jeszcze w programie wgranej żadnej wartości dla 'y'.
      if (sCoreCommand.substring(2, 4) == "up") Up(); //jeżeli nie są przeprowadzane żadne ruchy po polach, to wpadamy do funkcji up/down. one też działają na ruch iteracyjnie
      else if (sCoreCommand.substring(2, 6) == "down") Down();
    }

    if (d_y_temp == 0 || d_z_temp == 0 || d_kp_temp == 0) //dla pierwszego automatycznego ustawienia ramienia nie obliczaj przesuwu od poprzedniego ruchu
    { //tempy są zerami jak nie było jeszcze żadnego przejścia przez pętle ruchu, tj. nie ma zapisnego ruchu poprzedniego
      d_y_temp = y; LCD_pos.PrintPos("y", y); //tempy odpowiadają za pamiętanie poprzednio osiąganych 'y' i 'z'...
      d_z_temp = z; LCD_pos.PrintPos("z", z);  //...dzięki temu program zna drogę ramienia skąd-dokąd
      d_kp_temp = nKatPodstawa; LCD_pos.PrintPos("kp", nKatPodstawa); //PrintPosToLCD("kp", nKatPodstawa);
      b_przerywanie_petli = true; //dzięki tej zmiennej po złapaniu pozycji pierwszej i zarazem docelowej przy pierwszym ruchu program nie przechodzi bez sensu przez całą pętle ruchu, tylko...
      //...przerywa ją po 1 przejściu.
    }
    else //zatem dla każdego normalnego podanego ruchu
    { //oblicz części ruchu do dodawania w pętli ruchu:
      d_y_part = (y - d_y_temp) / n_wsp_ruchu; // floaty. minusowe wartosci dla zmiennej, jezeli punkt jest "z tyłu" (tzn. nowa wartość osi 'y', 'z', 'kp' jest mniejsza od poprzedniej).
      d_z_part = (z - d_z_temp) / n_wsp_ruchu;
      d_kp_part = (nKatPodstawa - d_kp_temp) / n_wsp_ruchu;
    }

    for (int i = 0; i < n_wsp_ruchu; i++) //wykonuj cząstkowe ruchy "n_wsp_ruchu" razy, aż ruch się wykona, lub do błędnych kątów serw. duży warunek.
    {
      d_y_temp += d_y_part; //od ostatniej pozycji powoli jedź do nowej podanej...
      d_z_temp += d_z_part;
      d_kp_temp += d_kp_part; //...Party (d_y_part, d_z_part, d_kp_part) są zerami w ruchu ustawczym (pierwszym)

      n_y_temp = (int) d_y_temp; LCD_pos.PrintPos("y", n_y_temp);  //do obliczeń potrzebuje integerów
      n_z_temp = (int) d_z_temp; //PrintPosToLCD w już zawarte poprawce pionowej
      n_kp_temp = (int) d_kp_temp; //!!! tu ppwinno być przeliczanie na 'x' z 'kp'

      if (i >= n_wsp_ruchu - 1) //docelowy zadany punkt. jeśli gdzieś powstałyby minimalne błędy, to tu są naprawiane
      { //... w sumie nie pamiętam co miał na celu ten warunek. pewnie jest już zbędny.
        n_y_temp = y; LCD_pos.PrintPos("y", n_y_temp);
        n_z_temp = z; //PrintPosToLCD w poprawce pionowej
        n_kp_temp = nKatPodstawa; //!!! tu ppwinno być przeliczanie na 'x' z 'kp'
      }

      Komunikaty.PokazInfo(INFO_KAT_KP, (String)d_kp_temp);
      Komunikaty.PokazInfo(INFO_ODLEGLOSC_Y, (String)n_y_temp);
      Komunikaty.PokazInfo(INFO_ODLEGLOSC_Z, (String)n_z_temp);

      //poniżej poprawka wysokosci - niech program myśli że jego zadane wartości są idealnie odwzorywane, a realnie obliczaj i ustawiaj kąty tak, by wyszło lepiej o skompensowany...
      //...zmierzony błąd
      n_z_temp += f_wektor_odchylek[n_y_temp - 100]; LCD_pos.PrintPos("z", n_z_temp); //dodaj różnicę wysokości podstawy chwytaka i planszy

      Komunikaty.PokazInfo(INFO_ODLEGLOSC_Z_UPGR, (String)n_z_temp);

      if (n_z_temp >= 236) //dla najdalszych bierek maxymalne możliwe podniesienie ramienia to 236 (dalej jest to poza polem roboczym manipulatora). powinno to być liczone w...
      { //...zalezności od możliwości, ale póki co jest to zablokowane dla wszystkich pozycji.
        n_z_temp = 235; LCD_pos.PrintPos("z", n_z_temp);
        //n_servo_speed = 255; //osiągnij prędkośc dla serw maxymalną, by szybko wyjść z warunku blokującego ruch (w tym przypadku serwa się nie ruszają, więc prędkość maxymalna...
        //...serw jest tylko zmienną obliczeniową by w ułamku sekundy wyjśc z funkcji delay) - coś się tu pierdoli
      }
      else //trochę naokoło ten kod poniżej. można się go pozbyć jak to się zrobi mądrzej
      {
        if (n_speed_changed == 0) n_servo_speed = SERVO_SPEED;  //predkosc serw podczas normalnej pracy (polecenia 'up' i 'down' zmieniają prędkość. tutaj trzeba to naprawiać)...
        else n_servo_speed = n_speed_changed;
        if (b_up == true || b_down == true) n_servo_speed = MOTOR_SPEED_SLOW;
      }

      /*y_kwadrat = n_y_temp * n_y_temp; 
      z0 = n_z_temp - z1; //odległość punktu P(y,z) od wysokośći na której jest servoB w pionie- założenie, że punkt nigdy nie jest poniżej serva (nigdy nie będzie)
      z0_kwadrat = z0 * z0; 
      L = sqrt(z0_kwadrat + y_kwadrat); //przekątna od servaB do punktu P(y,z)*/

      /*pre_alpha1_rad = (L * L + a * a - b * b) / (2 * L * a); //obliczanie alpha1 w radianach bez acos
      pre_alpha2_rad = n_y_temp / L; //obliczanie alpha2 w radianach bez acos 
      alpha_rad = acos(pre_alpha1_rad) + acos(pre_alpha2_rad); //cały kąt alpha w radianach 
      alpha = (180 / PI) * alpha_rad; //docelowy kąt alpha

      Komunikaty.PokazInfo(INFO_KAT_ALPHA, (String)alpha);

      beta_rad = acos((a * a + b * b - L * L) / (2 * a * b)); //obliczanie bety w radianach 
      beta = (180 / PI) * beta_rad; //kąt beta docelowy.

      if (beta >= 157) beta = 156; //!!servo beta ma słaby zakres!!! 

      Komunikaty.PokazInfo(INFO_KAT_BETA, (String)beta);

      if (b_up == true || b_down == true || sCoreCommand.substring(0, 1) == "(") //jeżeli ruch łapa idzie w dół/górę lub ruch odbywa się funkcją serwisową...
      { //... to licz kąt fi normalnie, tak by chwytak był zawsze prostopadły do planszy...
        fi = 270 - alpha - beta; //kąt fi docelowy
      }
      else fi = 179; //...a jeżeli nie to podnieś chwytak najwyżej jak się da, by przy przenoszeniu bierek nie chaczył o inne.

      if (fi == 179) Komunikaty.PokazInfo(INFO_KAT_FI, (String)fi);
      else Komunikaty.PokazInfo(INFO_KAT_FI, (String)(fi + n_fi_poprawka));*/

      if (alpha <= 180 && alpha >= 0 && beta <= 157 && beta >= 24 && fi <= 180 && fi >= 45) //dopuszczalne kąty.
        //servo beta dziala w przedziale od 24 do 157 stopni. servo fi minimum 45, bo by gniotło się o ramię przed nim
        //jeżeli wartości kątów mieszczą się w dopuszczalnych wartościach, to zezwól na ich ustalenie i kontynuuj obliczanie/ruch
      {
        servoA.UstawKat(n_kp_temp, static_cast<MOTOR_SPEED>(n_servo_speed)); /*servoA.write(n_kp_temp, n_servo_speed, false);*/ //tutaj to nie zadziala. musi być warunek dla n_kp_temp
        servoB.UstawKat(alpha, static_cast<MOTOR_SPEED>(n_servo_speed)); LCD_angle.PrintAngle("servoB", servoB.Kat());
        servoC.UstawKat(beta, static_cast<MOTOR_SPEED>(n_servo_speed)); LCD_angle.PrintAngle("servoC", servoC.Kat());
        //bardzo problematyczne jest ustawić kąt mechanicznie. zmieniam o n_fi_poprawka stopni do fi zawsze by wyszło tyle ile jest założone.
        servoD.UstawKat(fi + n_fi_poprawka, static_cast<MOTOR_SPEED>(n_servo_speed)); LCD_angle.PrintAngle("servoD", servoD.Kat()); //!!!z jakis powodow tutaj predkosc ustawiona jest na 35
      }
      else //jeżeli kąt wyskoczył poza dopuszczalne wartości, to ustaw bezpieczne wartości, pokaż gdzie był błąd w zmiennych i wyjdź z pętli
      {
        Serial.println("ERROR: Kat poza zakresem. Przerwanie ruchu i wyjscie z petli.");
        servoA.UstawKat(90, static_cast<MOTOR_SPEED>(n_servo_speed)); LCD_angle.PrintAngle("servoA", servoA.Kat());
        servoB.UstawKat(90, static_cast<MOTOR_SPEED>(n_servo_speed)); LCD_angle.PrintAngle("servoB", servoB.Kat());
        servoC.UstawKat(90, static_cast<MOTOR_SPEED>(n_servo_speed)); LCD_angle.PrintAngle("servoC", servoC.Kat());
        servoD.UstawKat(90, static_cast<MOTOR_SPEED>(n_servo_speed)); LCD_angle.PrintAngle("servoD", servoD.Kat());

        Komunikaty.ServoPozaZakresem();
        
        nKatPodstawa = 90; alpha = 90; beta = 90; fi = 90; y = b; z = z1 + a; //wartości jak z 'reset'
        LCD_pos.PrintPos("y", y); LCD_pos.PrintPos("z", z);
        i = n_wsp_ruchu - 1; //po tym wyjdź z petli
      }

      delay(15000 / (n_servo_speed * n_wsp_ruchu)); //po każdej iteracji czekaj chwilę, aby serwa zdążyły powoli dojechać. współczynnik wyznaczony empirycznie (tj. na oko).
      if (b_przerywanie_petli == true)
      { //dla wstepnego ustawienia ramion tylko raz licz katy serw
        b_przerywanie_petli = false;
        i = n_wsp_ruchu - 1; //owa zmienna 'i' przerwie pętle w kolejnym przejściu
      }
    }

    //wartości mimo iż są ok, przypisywane są jeszcze raz, bo możliwe że po wielu przejściach kąty będą się przesuwać o małe wartości
    d_kp_temp = nKatPodstawa; /*powtórne dla pierwszego ustawienia ramienia- nie szkodzi*/
    d_y_temp = y;
    d_z_temp = z;

    if (n_speed_changed == 0) n_servo_speed = SERVO_SPEED;  //predkosc serw podczas normalnej pracy (polecenia 'up' i 'down' zmieniają prędkość. tutaj trzeba to naprawiać)...
    else n_servo_speed = n_speed_changed; //...jeżeli jednak prędkość była zmieniana ręcznie to będzie ona wracała zawsze do podanej ręcznie wartości.

    if (b_up == true && Komunikaty.getSekwencjaRuchow()) //jeżeli łapa się podniosła podczas rozgrywania partii szachów, to podnieś łapę maxymalnie nad bierki...
    { //...poprzez danie maxymalnego kąta fi (wyprostowanie ramienia)
      servoD.UstawKat(179, MOTOR_SPEED_FAST, true); //parametr true powinien wykonać najpierw podniesienie zanim kod ruszy dalej
      delay(10); //nie wiem czemu to służy, ale używane tego przy wartości true na tutorialu, więc nie zaszkodzi
      LCD_angle.PrintAngle("servoD", servoD.Kat());
    }
    else if (b_down == false) //po przeniesieniu się nad planszą ustaw sobie już kąt do zejścia po bierkę
    {
      fi = 270 - alpha - beta;
      servoD.UstawKat(fi + n_fi_poprawka, MOTOR_SPEED_FAST);
      LCD_angle.PrintAngle("servoD", servoD.Kat());
    }

    if (Komunikaty.getSekwencjaRuchow() == true) //jeżeli mieliśmy do czynienia z ruchem generowanym z gry/ze strony...
    {
      if (Komunikaty.InfoDlaRdzenia() == "armUp2") //... i jeżeli był to ostatni ruch z sekwencji przemieszczania pionka...
      {
        servoA.UstawKat(110, static_cast<MOTOR_SPEED>(n_servo_speed)); LCD_angle.PrintAngle("servoA", servoA.Kat());  //... to serwem podstawy ustaw się na środku planszy ...
        Komunikaty.setSekwencjaRuchow(false); //... i nie wykonuj już więcej funkcji związanym z grą na stronie WWW...
      }
      //... a na końcu wyślij do core informację o tym jaki żądany ruch z core'a został wykonany:
      // jeżeli żądanie ruchu zawierało pole nad które miała się łapa ruszyć, to do informacji zwrotnej dopisz też informację o tym jakie to było pole (zbędna ochrona)...
      if (Komunikaty.InfoDlaRdzenia().substring(2, 7) == "moved") 
      Komunikaty.OdpowiedzNaRdzen(Komunikaty.InfoDlaRdzenia(), " " + Szachownica.getLiteraPola() + Szachownica.getCyfraPola());
      else Komunikaty.OdpowiedzNaRdzen(Komunikaty.InfoDlaRdzenia(), ""); //...a dla innych ruchów wyślij samą odpowiedź o wykonanym ruchu.
      // !!w rdzeniu (core) mam stare ustawienia wg których każda wiadomość kończy się dolarem. zobaczyć czy to coś psuje (nie widać by coś się złego działo).
    }

    if (b_down == false && Komunikaty.InfoDlaRdzenia() != "r_trashed") //jeżeli funkcja nie wykonywała ruchu typu down ani usuwania bierki
    { // warunek aby nie wywracało bierek skrajnych
      alpha += 8;
      servoB.UstawKat(alpha, static_cast<MOTOR_SPEED>(n_servo_speed), true); LCD_angle.PrintAngle("servoB", servoB.Kat()); //servoB.write(alpha, 6, true); PrintAngleToLCD("servoB"); //!! z powodu jakiegos bledu bylo tu ustawione predkosc 6, by bledu nie bylo widac
    }

    /*czyszczenie zmiennych*/
    b_up = false; b_down = false; //jeżeli był wykonywny ruch typu up/down to zresetuj te zmienne, by w kolejnym przejściu arduino nie zareagowało na nie.
    sCoreCommand = ""; //po przejściu przez cały program wyczyść tą zmienną, by w kolejnym przejściu nie został uruchomiony któryś warunek.
  }
}

void Open()
{
  nKatSzczeki = 102;
  servoE.UstawKat(nKatSzczeki, static_cast<MOTOR_SPEED>(n_servo_speed)); LCD_angle.PrintAngle("servoE", servoE.Kat());
  servoF.UstawKat(200 - nKatSzczeki, static_cast<MOTOR_SPEED>(n_servo_speed)); LCD_angle.PrintAngle("servoF", servoF.Kat());
  Komunikaty.PokazInfo(INFO_OPEN, (String)nKatSzczeki);
}

void Close()
{
  nKatSzczeki = 83;
  servoE.UstawKat(nKatSzczeki, static_cast<MOTOR_SPEED>(n_servo_speed)); LCD_angle.PrintAngle("servoE", servoE.Kat());
  servoF.UstawKat(200 - nKatSzczeki, static_cast<MOTOR_SPEED>(n_servo_speed)); LCD_angle.PrintAngle("servoF", servoF.Kat());
  Komunikaty.PokazInfo(INFO_CLOSE, (String)nKatSzczeki);
}

void Up()
{
  b_up = true;
  z = 230; //jedna z 2 możliwych 'z' możliwych docelowo do osiągnięcia w programie (inne są zbędne)
  n_servo_speed = MOTOR_SPEED_SLOW; //predkosc serw podczas podnoszenia od planszy
  Komunikaty.PokazInfo(INFO_UP, (String)z);
}

void Down()
{
  b_down = true;
  z = 158; //jw. w 'up'
  n_servo_speed = MOTOR_SPEED_SLOW; //predkosc serw podczas opadania ku planszy
  Komunikaty.PokazInfo(INFO_DOWN, (String)z);
}

void Trash()
{
  Komunikaty.PrzygotujOdpowiedzNaRdzen(sCoreCommand);
  nKatPodstawa = 175; //ustaw podstawe nad pundełkiem na zbite bierki...
  servoA.UstawKat(nKatPodstawa); LCD_angle.PrintAngle("servoA", servoA.Kat()); LCD_pos.PrintPos("x", -1); //speed ustawiony na sztywno
  sCoreCommand = "(170,240)"; //...i wyceluj tam na płaszczyźnie (y,z).
}

