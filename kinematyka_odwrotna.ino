#include <VarSpeedServo.h> //biblioteka odpowiedzialna za powolny ruch serw (nowa funkcja write() )
//dokonać odpowiednich konwersji. albo wyrzucić całą matematykę poza arduino do Qt
#include <math.h> //cosinusy, PI itd
#include <LiquidCrystal.h> //panele lcd

LiquidCrystal lcd1(32, 30, 28, 26, 24, 22);
LiquidCrystal lcd2(33, 31, 29, 27, 25, 23);

#define SERVO_POWER_PIN1 50 //zamiast pinów posługujemy się zdefiniowanymi słowami
#define SERVO_POWER_PIN2 51
#define POWER_LED_PIN 40
#define POWER_PIN 41
bool b_power_state; //do sprawdznia czy zasilacz jest włączony

VarSpeedServo servoA, servoB, servoC, servoD, servoE, servoF; //definicja serw wg specjalnej biblioteki
double alpha_rad, beta_rad; //kąty serw B i C w radianach
double fi1, fi2, fi3, fi_rad; //skomplikowane liczenie kąta fi w kilku krokach
float f_katPodstawa; //kąt podstawy- serwo podstawy. jako float
int n_katPodstawa = 90; int n_katSzczeki = 90; //kąty podstawy i szczęk jako integery. na starcie zdefiniowane na 90 stopni (środek zakresu pracy serwa)
double alpha = 90, beta = 90, fi = 90, fi_temp = fi, fi_last = fi; // kąty serw przy ładowaniu programu. jw.
double a = 180; double b = 184; double c = 142; //dlugosci ramion [mm]. dlugosc C mierzona do podstawy chwytaka. reszta to odległości pomiędzy orczykami serw
int z1 = 116; //[mm] odległość planszy od servaB (alpha) - pierwszy człon ruchumy ramienia wisi w powietrzu
unsigned long z0; //odległość punktu P(y,z) od wysokośći na której jest servoB w pionie (wartość obliczana dopiero jak podane jest "z" w "P").
double  L; //przekątna od servaB do punktu P(y,z). wartość obliczana dopiero jak podane jest "z" w "P".
unsigned long y_kwadrat, z0_kwadrat; //druga potega y i z0 (wartości inaczej się nie mieszczą w obliczeniach- wypadają poza zasięgi zmiennych)
int n_y, n_z; // wymagany punkt P(y,z) jako integery. najważniejsze wartości w programie
unsigned long y = 0, z; //skladowe punktu P(y,z) jako ogromne liczby. najważniejsze wartości w programie. ważne było by wartość 'y' była ustawiona na starcie na 0.
//uwaga: wartośći y, z (czy n_y, n_z) są tak naprawdę współczynnikami pozycji punktu P, a nie realnymi wartościami tego punktu w milimetrach w przestrzeni rzeczywistej (wartości
//liczone od podstawy ramienia). nie sprawdziłem tych wartości przy liczeniu kinematyki odwrotnej i odbiegają one o jakieś 10% od tego co się da obliczyć linijką. dlatego później w
//programie występują zmienne typu: d_kp_y, d_kp_x, d_b, d_c (oraz cały blok zmiennych potrzebnych do liczenia ich) na podstawie których liczony jest kąt podstawy i jest komparacja
//położenia pól planszy szachownicy w stosunku do punktu P(y,z) liczonego na współczynnikach.
double d_y_part = 0, d_z_part = 0; //zwiększane kawałki od aktualnego punktu P do nowopodanego punktu P (w iteracjach)
double d_y_temp = 0, d_z_temp = 0; //tymczasowy y,z do obliczen (używane przy d_y_part, d_z_part)
unsigned long n_y_temp, n_z_temp; //integery tempów do obliczeń (używane przy d_y_part, d_z_part)
String stringOne, Str_y, Str_z, Str_katSzczeki, Str_katPodstawy, Str_fi, Str_alpha, Str_beta, Str_servo_speed; //, readString; <- nie wiem czemu myślałem że zmienna readString jest
//potrzebna (świeciło się ładnie na pomarańczowo i było użyte w jakimś przykładzie na niesie. na wszelki wypadek zostawiam to tutaj). stringOne - jedna z ważniejszych zmiennych.
//zapisywane jest w niej to co przyjdzie po USB i program działa w opraciu o to co w niej jest. można zmienić tą nazwę bo w sumie głupia jest (użyta z przykładu z neta i tak zostało).
//reszta zmiennych to po prostu Stringi potrzebne przy konwersji z stringOne na wartości liczbowe (i tyczą się one wartości y,z ważnych do liczenia punktu P na płaszczyźnie (y,z).
double pre_alpha1_rad, pre_alpha2_rad, pre_beta_rad; //pomocnicze przy liczeniu kątów serw (problem z zasięgiem zmiennych)
bool b_up = false; bool b_down = false; //zmienne informują czy w danych ruchu ramienia było ono podnoszone, czy opuszczane (ważne dla prostowania serwa kąta fi, by nic nie wywrócić).
int n_servo_speed = 18; //predkosc dla serw podczas wstepnego ustawiania. wartość wykładnicza. im wyższe wartości tym szybciej chodzi (max działający = bodajże 127).
const int SERVO_SPEED = n_servo_speed; //podczas pracy ramienia prędkość ruchu serw jest zmieniana. by wracać to pierwotnej używamy tej stałej.
int n_speed_changed = 0; // program co zakończenie ruchu ustawia sobie zdefiniowaną stałą prędkość ruchu ramienia. jeżeli ręcznie zmienimy tą prędkość funkcją serwisową "speed ="...
//...to będzie utrzymywał prędkość tej tutaj zmiennej. 0 na starcie by omijać tą zmienną jak nic nie ruszymy.
int n_wsp_ruchu = 65; //współczynnik definiujący ilość pojedyńczych skoków/iteracji w ruchu ramienia (jego przenoszenia). wartość na wyczucie.
bool b_przerywanie_petli = false; //przerywa pętle po wstepnym ustawieniu serw (by wykonało tylko jedną iterację, anie wszystkie z "n_wsp_ruchu").
bool b_show_info = false; // jeżeli = true, to wypluwa na serial port tonę informacji o swoim stanie podczas działania. by działać z core'm musi być ustawione na false.
bool b_sekwencja_ruchow = false; //definiuje czy ruch jest spowodowany ręcznie (false), czy przez core (true). wartości core'a zmieniają tą zmienną.
bool b_znaki_koncow_linii = true; //definiuje czy dodajemy w funkcji znaki dla początku i końca wiadomości dla poprawnej komunikacji asynchronicznej z core'm
String Str_move_case = "none"; //zmienna po której komunikuje się arduino z core'em, tj. mówi mu co aktualnie za żądanych ruch wykonał.
int n_wsp_ks2 = 200; //pomocniczna zmienna tutaj, by nie wpisywac jej 5 razy w kodzie. na jej podstawie obliczany jest kąt serwa szczęki nr2
float f_wektor_odchylek[] = { -11, -10.9, -10.8, -10.7, -10.6, -10.5, -10.4, -10.3, -10.2, -10.1, -10, -9.7, -9.4, -9.1, -8.8, -8.5, -8.2, -7.9, -7.6, -7.3, -7, -7.3, -7.6, -7.9, -8.2,
                              -8.5, -8.8, -9.1, -9.4, -9.7, -10, -9.5, -9, -8.5, -8, -7.5, -7, -6.5, -6, -5.5, -5, -5, -5, -5, -5, -5, -5, -5, -5, -5, -5, -4.9, -4.8, -4.7, -4.6, -4.5, -4.4, -4.3, -4.2, -4.1, -4,
                              -4, -4, -4, -4, -4, -4, -4, -4, -4, -4, -3.7, -3.4, -3.1, -2.8, -2.5, -2.2, -1.9, -1.6, -1.3, -1, -0.9, -0.8, -0.7, -0.6, -0.5, -0.4, -0.3, -0.2, -0.1, 0, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6,
                              0.7, 0.8, 0.9, 1, 1.2, 1.4, 1.6, 1.8, 2, 2.2, 2.4, 2.6, 2.8, 3, 3.2, 3.4, 3.6, 3.8, 4, 4.2, 4.4, 4.6, 4.8, 5, 5.2, 5.4, 5.6, 5.8, 6, 6.2, 6.4, 6.6, 6.8, 7, 7.2, 7.4, 7.6, 7.8, 8, 8.2,
                              8.4, 8.6, 8.8, 9, 9.1, 9.2, 9.3, 9.4, 9.5, 9.6, 9.7, 9.8, 9.9, 10, 10.3, 10.6, 10.9, 11.2, 11.5, 11.8, 12.1, 12.4, 12.7, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13.5, 14, 14.5, 15,
                              15.5, 16, 16.5, 17, 17.5, 18, 18.1, 18.2, 18.3, 18.4, 18.5, 18.6, 18.7, 18.8, 18.9, 19, 19.2, 19.4, 19.6, 19.8, 20, 20.2, 20.4, 20.6, 20.8, 21, 21.4, 21.8, 22.2, 22.6, 23, 23.4, 23.8,
                              24.2, 24.6, 25, 25.3, 25.6, 25.9, 26.2, 26.5, 26.8, 27.1, 27.4, 27.7, 28, 27.9, 27.8, 27.7, 27.6, 27.5, 27.4, 27.3, 27.2, 27.1, 27, 27.9, 28.8, 29.7, 30.6, 31.5, 32.4, 33.3, 34.2, 35.1,
                              36
                            }; //wektor poprawek wysokości (oś z). wyliczony doświadczalnie (nigdy poprawki nie są idealne). wektor koryguje wartości z zakresu od y = 100 (1st element of array) do 340 mm (last)

/* zmienne dla liczenia kąta podstawy na podstawie podanego pola planszy: */
int n_wsp_m = 0; int n_wsp_n = 0; //interpretacje pól planszy szachownicy. "m" to liczby, "n" to litery (A1,H7,C3 itd..). "m" może operować wartościami od 3 do -4 (ważne dla obliczeń).
String Str_litera_pola, Str_cyfra_pola; //pole planszy szachownicy rozbite na 2 składowe i wrzucone do Stringów do dalszych przekształceń ( z char'ami miałem problemy).
float f_y0 = 114; float f_dl_pola = 28.6; float f_blad_srodka = 8.8; //na podstawie tych wartości i 4 skrajnych pól szachownicy liczone są położenia wszsytkich innych pól planszy w grze.
//zmienne zostały zmierzone linijką, a potem metodami sztucznej inteligencji w matlabie dociągnięte do takich wartości, by ze wzoru na wszsytkie pola szachownicy najlepiej wychodziły te
//4 krańcowe pola wzięte do obliczeń.
double d_kp_y, d_kp_x; //realne wartości 'y' i 'x' potrzebne by wyliczyć poprawny kąt podstawy dla danego pola planszy (czyli wartość x). nie ma tu żadnego powiązania ze zmiennymi:
//y, z, n_y, n_z (+ich stringi) które służą do liczenia położenia punktu P na płaszczyźnie (y,z).

/*zmienne do liczenia długości 'y' na podstawie podanych pól planszy*/
double d_b = 0, d_c = 0; //specjalne osobne 'x' i 'y' do obliczania standardowego 'y' punktu P płaszczyzny (y,z). Za pomocą wzorów na 'x' i 'kp' wyliczono 4 skrajne pola planszy
//(w matlabie) i na podstawie tych punktów wyliczono środki reszty pól szachownicy (też w matlabie). punkty te są zinterpretowane jako 2 wektory, gdzie A1=(d_b_array[0],d_c_array[0])...
//A8=(d_b_array[7],d_c_array[7]), B1=(d_b_array[8],d_c_array[8])... H8=(d_b_array[63],d_c_array[63]).
//matlab, chyba wyliczane to jest na podstawie 'y' i 'x' z kp, potem SI->wsp. wzoru liczace wektory i na ich podstawie liczony wsp. y do P(y,z)
double d_b_array[] = {112.4, 141.8, 171.1, 200.5, 229.9, 259.3, 288.6, 318.0, 112.1, 141.5, 170.9, 200.3, 229.7, 259.1, 288.5, 317.9, 111.9, 141.3, 170.7, 200.1, 229.5, 259.0, 288.4,
                      317.8, 111.6, 141.0, 170.5, 199.9, 229.4, 258.8, 288.3, 317.7, 111.3, 140.8, 170.3, 199.7, 229.2, 258.7, 288.1, 317.6, 111.0, 140.5, 170.0, 199.5, 229.0, 258.5, 288.0, 317.5, 110.8,
                      140.3, 169.8, 199.3, 228.8, 258.4, 287.9, 317.4, 110.5, 140.0, 169.6, 199.1, 228.7, 258.2, 287.8, 317.3
                     };
double d_c_array[] = {107.4, 107.8, 108.2, 108.6, 109.0, 109.4, 109.8, 110.2, 79.4, 79.7, 79.9, 80.2, 80.5, 80.8, 81.0, 81.3, 51.4, 51.6, 51.7, 51.8, 52.0, 52.1, 52.2, 52.4, 23.4, 23.4,
                      23.4, 23.4, 23.5, 23.5, 23.5, 23.5, -4.5, -4.7, -4.8, -4.9, -5.1, -5.2, -5.3, -5.5, -32.5, -32.8, -33.1, -33.3, -33.6, -33.8, -34.1, -34.4, -60.5, -60.9, -61.3, -61.7, -62.1, -62.5,
                      -62.9, -63.3, -88.5, -89.0, -89.6, -90.1, -90.6, -91.1, -91.7, -92.2
                     };

void setup() //kod kóry wykona się raz
{
  pinMode(SERVO_POWER_PIN1, OUTPUT); //ustawianie tego pinu jako wyjście, tj. będziemy tu czymś sterować. pin włącza przekaźnikiem zasilanie na serva
  pinMode(SERVO_POWER_PIN2, OUTPUT);
  digitalWrite(SERVO_POWER_PIN1, HIGH); //przekaźnik działa na odwrót, zatem by był on na starcie wyłączony to dajemy na jego pin stan wysoki
  digitalWrite(SERVO_POWER_PIN2, HIGH);

  //ustaw zasilacz wyłączony
  pinMode(POWER_LED_PIN, OUTPUT); //dioda led zasilacza. póki co tylko tu w kodzie
  digitalWrite(POWER_LED_PIN, LOW);
  pinMode(POWER_PIN, OUTPUT);
  digitalWrite(POWER_PIN, LOW); //mechanicznie nic nie podpięte, ale programowo trzeba włączać (dać na pin zasilania stan wysoki) by poszedł prąd na serwa- info dalej

  lcd1.begin(16, 2);
  lcd2.begin(20, 2);
  lcd1.clear();
  lcd2.clear();

  servoA.attach(2); servoB.attach(3); servoC.attach(4); servoD.attach(5); servoE.attach(6); servoF.attach(7); //podpinanie serwomechanizmów to tych pinów. piny muszą być PWN
  servoA.write(n_katPodstawa, n_servo_speed, false); servoB.write(alpha, n_servo_speed, false); servoC.write(beta, n_servo_speed, false); //ustawienie wstępnej pozycji serw by w razie...
  servoD.write(fi + 7, n_servo_speed, false); servoE.write(n_katSzczeki, n_servo_speed, false); servoF.write(n_wsp_ks2 - n_katSzczeki, n_servo_speed, false); //...awarii kodu łapa nie runęła na szachy
  PrintAngleToLCD("servoA"); PrintAngleToLCD("servoB"); PrintAngleToLCD("servoC"); PrintAngleToLCD("servoD"); PrintAngleToLCD("servoE"); PrintAngleToLCD("servoF");
  stringOne = "(184,296)"; //ustawienie dla pierwszego uruchomienia łapy. dla tego punktu P(y,z) wszystkie ważne serwa powinny mieć ustawione 90 stopni po przejściu przez kod
  Serial.begin(9600); //rozpocznij komunikację po usb z prędkośćią 9600 bitów/s
}

void loop() //wieczna główna pętla programu
{
  while (Serial.available()) //gdy pojawi się na porcie jakaś wiadomość...
  {
    stringOne = Serial.readString(); //... zapisz ją w tej zmiennej.
  }

  //poniżej ogromne wielokrotnie zagnieżdżone funkcje warunkowe IF sprawdzające co przyszło w zmiennej stringOne...
  if (stringOne.substring(3, 5) == "]f") //jeżeli wiadomość to np. [a2]f to arduino rozpoczyna wymianę danych z core'm
  { //wiadomość oznacza żądanie przeniesienia pionka "z pozycji (f-from)". w tym ruchu ustawiane jest ramie w pukncie P nad pionkiem do podniesienia.
    b_show_info = false; //na wszelki wypadek wyłącz pokazywanie innych informacji (rozwaliły by komunikację)
    b_sekwencja_ruchow = true; //rozpoczęto rozmowe z core'm, więc niektóre ruchy wykonywać mają się trochę inaczej
    Str_move_case = "movedFrom"; //zostaje ustawiona zmienna, która po wykonaniu ruchu przez łapę zostanie wysłana core, jako potwierdzenie tego co miało się wykonać (i czekanie na...
    //...kolejny sygnał z sekwencji ruchów
  }
  //open1 przechwytywane w substringu "open" //pierwsze otworzenie szczęk ramienia...
  else if (stringOne.substring(0, 5) == "down1") Str_move_case = "armDown1"; //...pierwsze zejście ramienia na dół...
  //close1 przechwytywane w substringu "close" //...pierwsze zamknięcie szczęk ramienia na bierce...
  else if (stringOne.substring(0, 3) == "up1") Str_move_case = "armUp1"; //...pierwsze uniesienie ramienia ku górze...
  else if (stringOne.substring(3, 5) == "]t") Str_move_case = "movedTo"; //...przeniesienie łapy nad pole planszy na którym przechwycona bierka ma wylądować...
  else if (stringOne.substring(0, 5) == "down2") Str_move_case = "armDown2"; //...drugie zejście na dół (na pole docelowe)...
  //open2 przechwytywane w substringu "open" //...drugie otwarcie szczęk ramienia...
  else if (stringOne.substring(0, 3) == "up2") Str_move_case = "armUp2"; //...odjechanie na górę i koniec. czekanie na kolejne komendy.

  //usuwanie pionków z planszy:
  else if (stringOne.substring(3, 5) == "]r") //sekwencja poprzedza zwykłe przemieszczanie bierki. ma to miejsce, gdy bierka ma wylądować na polu na którym jest bierka wroga.
  { //o tym czy na bitym polu na pewno jest bierka wroga decyduje program szachowy, który przed ruchami arduino bada czy ruch może być wykonany.
    b_show_info = false;
    b_sekwencja_ruchow = true;
    Str_move_case = "movedToR";
  }
  //openR1 przechwytywane w substringu "open"
  else if (stringOne.substring(0, 5) == "downR") Str_move_case = "downR";
  //closeR przechwytywane w substringu "close"
  else if (stringOne.substring(0, 3) == "upR") Str_move_case = "armUpR";
  else if (stringOne.substring(0, 6) == "trashR")
  {
    n_katPodstawa = 165; //ustaw podstawe nad pundełkiem na zbite bierki...
    servoA.write(n_katPodstawa, n_servo_speed, false); PrintAngleToLCD("servoA"); PrintPosToLCD("x", -1);
    stringOne = "(170,240)"; //...i wyceluj tam na płaszczyźnie (y,z).
    Str_move_case = "trashedR";
  }
  //openR2 przechwytywane w substringu "open"

  //w powyższym warunku głównie była zmieniana wartość zmiennej Str_move_case. tutaj jest wykonywana reszta/większa część ruchów, gdzie np. wartość up2 wpada do warunku wyżej i tu.
  if (stringOne.substring(0, 5) == "reset") //funckja serwisowa. ustawia serwa na pozycję startową
  {
    AnswerToCore("","Reset: kat szczeki = 90, kat podstawy = 90");
    y = b; z = z1 + a; PrintPosToLCD("y", y); PrintPosToLCD("z", z); //ustawienia wynikajace z katow = 90
    stringOne = "(184,296)"; //to samo co wyzej
    n_katSzczeki = 90; n_katPodstawa = 90;
    servoA.write(n_katPodstawa, n_servo_speed, false); PrintAngleToLCD("servoA"); PrintPosToLCD("x", -1);
    servoE.write(n_katSzczeki, n_servo_speed, false); PrintAngleToLCD("servoE");
    servoF.write(n_wsp_ks2 - n_katSzczeki, n_servo_speed, false); PrintAngleToLCD("servoF");
  }
  else if (stringOne.substring(0, 7) == "speed =") //funckja serwisowa. ustawia prędkość przemieszczania się ramienia (bez up/down)
  {

    Str_servo_speed = stringOne.substring(8);
    if (Str_servo_speed.toInt() <= 255 && Str_servo_speed.toInt() >= 1) //z tego co wiem wartości powyżej 127 są takie same, albo nie widać różnicy. w każdym razie przyjmie max 255.
    {
      n_servo_speed = Str_servo_speed.toInt();
      n_speed_changed = n_servo_speed;
      Serial.print("servo speed = "); Serial.println(n_servo_speed);
    }
    else Serial.println(" servo_speed podany poza zakresem <1,255>");
    stringOne = ""; //prewencyjnie czyść tą zmienną. jest ona też czyszczona na końcu. bez tego arduino się zatnie wykonując w kółko to samo.
  }
  else if (stringOne.substring(0, 7) == "alpha =") //funckja serwisowa. ustaw kąt silnika B (alfa)
  {
    Str_alpha = stringOne.substring(8);
    alpha = Str_alpha.toInt();
    if (alpha <= 180 && alpha >= 0)
    {
      servoB.write(alpha, n_servo_speed, false);
      PrintAngleToLCD("servoB");
      PrintPosToLCD("y", -1); //ruszenie ręcznie kąta alpha gubi pozycję (y,z)
      PrintPosToLCD("z", -1);
      Serial.print("alpha = ");
      Serial.println(alpha);
    }
    else Serial.println("alpha podany poza zakresem <0,180>");
    stringOne = "";
  }
  else if (stringOne.substring(0, 6) == "beta =") //funckja serwisowa. ustaw kąt silnika C (beta)
  {
    Str_beta = stringOne.substring(7);
    beta = Str_beta.toInt();
    if (beta <= 157 && beta >= 24) //servo beta dziala w przedziale do 157 stopni
    {
      servoC.write(beta, n_servo_speed, false);
      PrintAngleToLCD("servoC");
      PrintPosToLCD("y", -1); //ruszenie ręcznie kąta beta gubi pozycję (y,z)
      PrintPosToLCD("z", -1);
      Serial.print("beta = ");
      Serial.println(beta);
    }
    else Serial.println("beta podana poza zakresem <23,157>"); //nienajlepsze serwo powoduje wąski przedział działających wartości
    stringOne = "";
  }
  else if (stringOne.substring(0, 4) == "ks =") //funckja serwisowa. ustaw kąt silnika E i F (szczęk)
  {
    Str_katSzczeki = stringOne.substring(5);
    n_katSzczeki = Str_katSzczeki.toInt();
    if (n_katSzczeki >= 82 && n_katSzczeki <= 131) //nie potrzeba by działało dla szerszych wartości
    {
      servoE.write(n_katSzczeki, n_servo_speed, false); PrintAngleToLCD("servoE");
      servoF.write(n_wsp_ks2 - n_katSzczeki, n_servo_speed, false); PrintAngleToLCD("servoF"); //drugie serwo w szczękach uastawia sobie kąt na odwrót, by współgrać z tym pierwszym. współczynnik zdobyty doświadczalnie
      Serial.print("kat szczeki = "); Serial.println(n_katSzczeki);
    }
    else Serial.println("katSzczeki podany poza zakresem <82,131>");
    stringOne = "";
  }
  else if (stringOne.substring(0, 4) == "kp =") //funckja serwisowa. ustaw kąt silnika A (podstawy)
  {
    Str_katPodstawy = stringOne.substring(5);
    n_katPodstawa = Str_katPodstawy.toInt();
    if (n_katPodstawa >= 15 && n_katPodstawa <= 165)
    {
      servoA.write(n_katPodstawa, n_servo_speed, false);
      PrintAngleToLCD("servoA");
      PrintPosToLCD("x", -1); //ruszenie ręcznie kąta podstawy gubi wszystkie pozycje
      PrintPosToLCD("y", -1);
      PrintPosToLCD("z", -1);
      Serial.print("kat podstawy = ");
      Serial.println(n_katPodstawa);
    }
    else Serial.println("kat podstawy podany poza zakresem <15,165>"); //poza zakresem tym nie bardzo reagowało serwo
    stringOne = "";
  }
  else if (stringOne.substring(0, 4) == "fi =") //funckja serwisowa. ustaw kąt silnika D (fi)
  {
    Str_fi = stringOne.substring(5);
    fi = Str_fi.toInt();
    if (fi >= 45 && fi <= 180) {
      servoD.write(fi, n_servo_speed, false);
      PrintAngleToLCD("servoD");
      Serial.print("fi = ");
      Serial.println(fi);
      Serial.print("Brak poprawki fi o +7");
    }
    else Serial.println("fi podany poza zakresem <45,180>");
    stringOne = "";
  }
  else if (stringOne.substring(0, 4) == "open") //otwieranie szczęk w celu odłożenia bierki na planszy, albo zejścia po nią
  {
    n_katSzczeki = 102;
    servoE.write(n_katSzczeki, n_servo_speed, false); PrintAngleToLCD("servoE");
    servoF.write(n_wsp_ks2 - n_katSzczeki, n_servo_speed, false); PrintAngleToLCD("servoF");
    if (b_show_info == true) {
      Serial.print("opened. katSzczeki = ");
      Serial.println(n_katSzczeki);
    }
    //poniżej warunki gdy jest wykonywany oficjalny ruch bierki podczas rozgrywki
    if (stringOne.substring(0, 5) == "open1" && b_sekwencja_ruchow == true) AnswerToCore("","opened1"); //nie wiem czy te prinln nie powinny być na samym końcu pętli. póki co działa.
    else if (stringOne.substring(0, 5) == "open2" && b_sekwencja_ruchow == true) AnswerToCore("","opened2");
    else if (stringOne.substring(0, 6) == "openR1" && b_sekwencja_ruchow == true) AnswerToCore("","openedR1");
    else if (stringOne.substring(0, 6) == "openR2" && b_sekwencja_ruchow == true) AnswerToCore("","openedR2");
    stringOne = "";
  }
  else if (stringOne.substring(0, 5) == "close") //analogicznie do open
  {
    n_katSzczeki = 85;
    servoE.write(n_katSzczeki, n_servo_speed, false); PrintAngleToLCD("servoE");
    servoF.write(n_wsp_ks2 - n_katSzczeki, n_servo_speed, false); PrintAngleToLCD("servoF");
    if (b_show_info == true) {
      Serial.print("closed. katSzczeki = ");
      Serial.println(n_katSzczeki);
    }
    if (stringOne.substring(0, 6) == "close1" && b_sekwencja_ruchow == true) AnswerToCore("","closed1");
    else if (stringOne.substring(0, 6) == "closeR" && b_sekwencja_ruchow == true) AnswerToCore("","closedR");
    stringOne = "";
  }
  else if (stringOne.substring(0, 7) == "turn on" || stringOne.substring(0, 7) == "turn_on" || stringOne.substring(0, 6) == "turnon")
  { //daj zasilanie na serwa...
    b_power_state = digitalRead(POWER_PIN); //(nie używane póki co. nie wiem czy zadziała, bo ten pin był ustawiony bodajże jako ustalający, a nie odczytujący (input/output), chyba że
    //tu jest rozpatrywany inny pin. sprawdzić póxniej)
    if (b_power_state == HIGH) //...pod warunkiem, że zasilacz jest włączony, tj. zasilacz dostał komendę power on, albo odczytał na pinie że działał (miał napięcie na zasilaczu).
    {
      digitalWrite(SERVO_POWER_PIN1, LOW);
      digitalWrite(SERVO_POWER_PIN2, LOW);
      AnswerToCore("","turn on servo power");
    }
    else AnswerToCore("","ERROR: power supply is turned off.");
    stringOne = "";
  }
  else if (stringOne.substring(0, 8) == "turn off" || stringOne.substring(0, 8) == "turn_off" || stringOne.substring(0, 7) == "turnoff")
  { //wyłączenie serw. jeżeli nie jest łapa dobrze oparta, to runie ona o podłoże. wprowadzić funkcję auto podpierania przed wyłączeniem.
    digitalWrite(SERVO_POWER_PIN1, HIGH);
    digitalWrite(SERVO_POWER_PIN2, HIGH);
    AnswerToCore("","turn off servo power");
    stringOne = "";
  }
  else if (stringOne.substring(0, 8) == "power on" || stringOne.substring(0, 8) == "power_on" || stringOne.substring(0, 7) == "poweron")
  { //włącz zasilacz (jeżeli nie jest włączony ręcznie)
    digitalWrite(POWER_LED_PIN, HIGH);
    digitalWrite(POWER_PIN, HIGH);
    AnswerToCore("","power on");
    stringOne = "";
  }
  else if (stringOne.substring(0, 9) == "power off" || stringOne.substring(0, 9) == "power_off" || stringOne.substring(0, 8) == "poweroff")
  { //analogicznie do włącz
    digitalWrite(POWER_LED_PIN, LOW);
    digitalWrite(POWER_PIN, LOW);
    AnswerToCore("","power off");
    stringOne = "";
  }
  else if (stringOne.substring(0, 7) == "info on" || stringOne.substring(0, 7) == "info_on" || stringOne.substring(0, 6) == "infoon")
  { //pokazuj stan wszystkiego co się dzieje na procku (wyrzucaj info na port). włączamy do prac serwisowych. włączenie gry automatycznie wyłączy.
    b_show_info = true;
    b_znaki_koncow_linii = false; //usuwaj znaki początku i końca linii w wiadomościach do core
    AnswerToCore("","info on");
    stringOne = "";
  }
  else if (stringOne.substring(0, 8) == "info off" || stringOne.substring(0, 8) == "info_off" || stringOne.substring(0, 7) == "infooff")
  { //wyłącz info- analogicznie do tego powyżej. arduino jest po starcie na "info off"
    b_show_info = false;
    b_znaki_koncow_linii = true; //pokazuj znaki początku i końca linii w wiadomościach do core
    AnswerToCore("","info off");
    stringOne = "";
  }

  //poniżej ogromny blok funkcji dla takich wartości stringOne, które powodują ruch ramienia w wielu przejściach/iteracjach/mini-ruchach
  else if (stringOne.substring(0, 1) == "(" || stringOne.substring(0, 1) == "[" ||
           stringOne.substring(0, 2) == "up" || stringOne.substring(0, 4) == "down")
  {
    if (stringOne.substring(0, 1) == "(" || stringOne.substring(0, 1) == "[")
    {
      if (stringOne.substring(0, 1) == "(") //funkcja serwisowa. warunek czyta wartość 'y', 'z' dla wartości stringOne z formatu: '(yyy,zzz)' oraz '(yyy,zzz,A1)'.
      {
        Str_y = stringOne.substring(1, 4); // y
        Str_z = stringOne.substring(5, 8); // z
        n_y = Str_y.toInt(); n_z = Str_z.toInt(); //zmień odczytany string na int...
        y = n_y; z = n_z; //...a teraz zmień odczyt na unsigned long
        if (z < 150) z = 150;
      }
      if (stringOne.substring(11, 12) == ")" || stringOne.substring(3, 4) == "]") //wartośc z nawiasem okrągłym serwisowa, z kwadratowym służaca do obsługi gry.
      { //warunek czyta wartość pola na planszy dla wartości stringOne z formatu: '(yyy,zzz,A1)' lub '[a1]'.
        if (b_show_info == true) Serial.println("Chess square statement met");
        if (stringOne.substring(11, 12) == ")") Str_litera_pola = stringOne.substring(9, 10); //sprawdź jaka podana jest litera pola dla funkcji serwisowych...
        else if (stringOne.substring(3, 4) == "]") Str_litera_pola = stringOne.substring(1, 2); //... i funkcji obsługi gry.
        if (b_show_info == true) {
          Serial.print("litera pola = ");
          Serial.println(Str_litera_pola);
        }
        if (Str_litera_pola == "a" || Str_litera_pola == "A") {
          n_wsp_m = 3;  //zamień tą literę na współczynnik n_wsp_m (jego wartośc jest ważna dla obliczeń)...
          Str_litera_pola = "a";
        }
        else if (Str_litera_pola == "b" || Str_litera_pola == "B") {
          n_wsp_m = 2;  //...oraz dla ułatwienia jeżeli podano format wielkich liter, to zamień na małe.
          Str_litera_pola = "b";
        }
        else if (Str_litera_pola == "c" || Str_litera_pola == "C") {
          n_wsp_m = 1;
          Str_litera_pola = "c";
        }
        else if (Str_litera_pola == "d" || Str_litera_pola == "D") {
          n_wsp_m = 0;  //wartości n_wsp_m wynikają z tego, że podstawa ramienia jest mniej więcej...
          Str_litera_pola = "d";
        }
        else if (Str_litera_pola == "e" || Str_litera_pola == "E") {
          n_wsp_m = -1;  //...na środku planszy i odległość na osi 'x' liczymy z udziałem mnożnika...
          Str_litera_pola = "e";
        }
        else if (Str_litera_pola == "f" || Str_litera_pola == "F") {
          n_wsp_m = -2;  //...pól w jedną lub drugą stronę. prawie te same obliczenia są w pliku...
          Str_litera_pola = "f";
        }
        else if (Str_litera_pola == "g" || Str_litera_pola == "G") {
          n_wsp_m = -3;  //...matlabowskim (załaczonym)
          Str_litera_pola = "g";
        }
        else if (Str_litera_pola == "h" || Str_litera_pola == "H") {
          n_wsp_m = -4;
          Str_litera_pola = "h";
        }
        else Serial.println("ERROR: Złe podane pole planszy.");
        if (b_show_info == true) {
          Serial.print("m = ");
          Serial.println(n_wsp_m);
        }
        if (stringOne.substring(11, 12) == ")") Str_cyfra_pola = stringOne.substring(10, 11); //sprawdź jaka podana jest cyfra pola dla funkcji serwisowych...
        else if (stringOne.substring(3, 4) == "]") Str_cyfra_pola = stringOne.substring(2, 3); //...i dla funkcji obsługi gry.
        n_wsp_n = Str_cyfra_pola.toInt(); //bezpośrednie przypisanie
        if (b_show_info == true) {
          Serial.print("n = ");
          Serial.println(n_wsp_n);
        }
        d_kp_y = f_y0 + (f_dl_pola * n_wsp_n); //z rzeczywistych zależności oblicz składową 'y' dla obliczenia kąta podstawy
        if (b_show_info == true) {
          Serial.print("y_kp = ");
          Serial.println(d_kp_y);
        }
        if (n_wsp_m >= 0) //dla pól A,B,C,D
        {
          d_kp_x = f_dl_pola * n_wsp_m + (f_blad_srodka + f_dl_pola / 2); PrintPosToLCD("x", d_kp_x); //oblicz z rzeczywistych zależności składową 'x' dla obliczenia kąta podstawy
          if (b_show_info == true) {
            Serial.print("x = ");
            Serial.println(d_kp_x);
          }
          f_katPodstawa = (180 / PI) * atan(d_kp_y / d_kp_x); //oblicz szukany kąt podstawy z magicznych matematycznych zależności
          n_katPodstawa = round(f_katPodstawa); //zaokrąglamy ręcznie, bo i tak to się stanie w funkcji write(), a tu funkcja round() zrobi to lepiej (na zasadzie ceil/floor).
        }
        else //dla pól E,F,G,H
        { //wzory są tutaj trochę inne, bo są "z drugiej strony planszy". na inne wzory wpływa też zasięg kątów w tangensie
          d_kp_x  = f_dl_pola * (-n_wsp_m - 1) + f_dl_pola / 2 - f_blad_srodka; PrintPosToLCD("x", d_kp_x);
          if (b_show_info == true) {
            Serial.print("x = ");
            Serial.println(d_kp_x);
          }
          f_katPodstawa = 180 - ((180 / PI) * atan(d_kp_y / d_kp_x));
          n_katPodstawa = round(f_katPodstawa);
        }
        servoA.write(n_katPodstawa, n_servo_speed, false); PrintAngleToLCD("servoA"); //obliczony kąt podstawy w końcu jest ustawiany nań
        if (b_show_info == true) {
          Serial.print("kp = ");
          Serial.println(n_katPodstawa);
        }
        if (stringOne.substring(0, 1) == "[") //mamy składową osi 'x' (z 'kp'). wartość osi 'z' wynika tylko z funkcji up/down. zostaje tutaj wyliczenie składowej ostatniej osi 'y'.
        {
          //współczynniki do poniższego wzoru obliczone podobnie jak te przy wzorze na 'kp'. najpierw trzeba było znaleść wzór określający wszsytkie środki pól na planszy...
          //...,współczynniki znalezione w matlabie metodami SI za pomocą 4 skrajnych pól planszy. obliczone wzory wrzucone do 2 wektorów opisujących wszsytkie środki pół planszy.
          //y = sqrt(pow((115.93-((-n_wsp_m+3)*30.349)),2)+pow((103.57+((n_wsp_n-1)*30.349)),2)); //c=115.93; b=103.57; x=212.44/7;
          d_b = d_b_array[((-n_wsp_m + 3) * 8) + n_wsp_n - 1]; //składowa pionowa
          d_c = d_c_array[((-n_wsp_m + 3) * 8) + n_wsp_n - 1]; //składowa pozioma
          y = sqrt(d_b * d_b + d_c * d_c); //wartość której szukamy- przekątna dwóch składowych powyżej
          if (b_show_info == true) {
            Serial.print("y [mm] = ");  //pokaż obliczoną odległość y
            Serial.println(y);
          }
        }
      }
    }
    else
    {
      if (y == 0) y = b; //jeżeli po odpaleniu ramienia nie była wprowadzona żadna wartość y, to ustaw domyślną. to jest ważne gdy po odpaleniu ramienia pierwszą rzeczą jaką...
      //...chcemy zrobić to podnieśc lub opóścić ramię, a nie ma jeszcze w programie wgranej żadnej wartości dla 'y'.
      if (stringOne.substring(0, 2) == "up") //jeżeli nie są przeprowadzane żadne ruchy po polach, to wpadamy do funkcji up/down. one też działają na ruch iteracyjnie
      {
        b_up = true;
        z = 230; //jedna z 2 możliwych 'z' możliwych docelowo do osiągnięcia w programie (inne są zbędne)
        n_servo_speed = 2; //predkosc serw podczas podnoszenia od planszy
        if (b_show_info == true)
        {
          Serial.print("up: ");
          Serial.print("y [mm] = "); Serial.print(y);
          Serial.print(", z [mm] = "); Serial.println(z);
        }
      }
      else if (stringOne.substring(0, 4) == "down")
      {
        b_down = true;
        z = 158; //jw. w 'up'
        n_servo_speed = 2; //predkosc serw podczas opadania ku planszy
        if (b_show_info == true)
        {
          Serial.println("down: ");
          Serial.print("y [mm] = "); Serial.print(y);
          Serial.print(", z [mm] = "); Serial.println(z);
        }
      }
    }

    if (d_y_temp == 0 || d_z_temp == 0) //dla pierwszego automatycznego ustawienia ramienia nie obliczaj przesuwu od poprzedniego ruchu
    { //tempy są zerami jak nie było jeszcze żadnego przejścia przez pętle ruchu, tj. nie ma zapisnego ruchu poprzedniego
      d_y_temp = y; PrintPosToLCD("y", y); //tempy odpowiadają za pamiętajnie poprzednio osiąganych 'y' i 'z'...
      d_z_temp = z; PrintPosToLCD("z", z); //...dzięki temu program zna drogę ramienia skąd-dokąd
      b_przerywanie_petli = true; //dzięki temu po złapaniu pozycji pierwszej i zarazem docelowej przy pierwszym ruchu program nie przechodzi bez sensu przez całą pętle ruchu, tylko...
      //...przerywa ją po 1 przejściu.
    }
    else //zatem dla każdego normalnego podanego ruchu
    { //oblicz części ruchu do dodawania w pętli ruchu:
      d_y_part = (y - d_y_temp) / n_wsp_ruchu; // floaty. minusowe wartosci dla zmiennej, jezeli punkt jest "z tyłu" (nowa wartość osi 'y' lub 'z' jest mniejsza od poprzedniej).
      d_z_part = (z - d_z_temp) / n_wsp_ruchu;
    }

    for (int i = 0; i < n_wsp_ruchu; i++) //wykonuj cząstkowe ruchy "n_wsp_ruchu" razy, aż ruch się wykona, lub do błędnych kątów serw. duży warunek.
    {
      d_y_temp += d_y_part; //od ostatniej pozycji powoli jedź do nowej podanej
      d_z_temp += d_z_part; //party (d_y_part, d_z_part) są zerami w ruchu ustawczym (pierwszym)
      n_y_temp = (int) d_y_temp; PrintPosToLCD("y", n_y_temp); //do obliczeń potrzebuje integerów
      n_z_temp = (int) d_z_temp; //PrintPosToLCD w poprawce pionowej

      if (i >= n_wsp_ruchu - 1) //docelowy zadany punkt. jeśli gdzieś powstałyby minimalne błędy, to tu są naprawiane
      { //... w sumie nie pamiętam co miał na celu ten warunek. pewnie jest już zbędny.
        n_y_temp = y; PrintPosToLCD("y", n_y_temp);
        n_z_temp = z; //PrintPosToLCD w poprawce pionowej
      }
      if (b_show_info == true)
      {
        Serial.print("y = "); Serial.print(n_y_temp);
        Serial.print(", z = "); Serial.print(n_z_temp); //wysokość będzie podawana ta, którą dopiero chcemy realnie osiągnąć poprzez kompensację błędu chwilę dalej, a którą zadaliśmy
      }

      //poniżej poprawka wysokosci - niech program myśli że jego zadane wartości są idealnie odwzorywane, a realnie obliczaj i ustawiaj kąty tak, by wyszło lepiej o skompensowany...
      //...zmierzony błąd
      n_z_temp += f_wektor_odchylek[n_y_temp - 100]; PrintPosToLCD("z", n_z_temp); //dodaj różnicę wysokości podstawy chwytaka i planszy
      if (b_show_info == true) {
        Serial.print(", z_upgr = ");
        Serial.print(n_z_temp);
      }
      if (n_z_temp >= 236) //dla najdalszych bierek maxymalne możliwe podniesienie ramienia to 236 (dalej jest to poza polem roboczym manipulatora). powinno to być liczone w...
      { //...zalezności od możliwości, ale póki co jest to zablokowane dla wszystkich pozycji.
        n_z_temp = 235; PrintPosToLCD("z", n_z_temp);
        if (b_show_info == true) Serial.print("!!!");
      }

      z0 = n_z_temp - z1; //odległość punktu P(y,z) od wysokośći na której jest servoB w pionie- założenie, że punkt nigdy nie jest poniżej serva (nigdy nie będzie)
      z0_kwadrat = z0 * z0; //Serial.print("z0^2 = "); Serial.println(z0_kwadrat);
      y_kwadrat = n_y_temp * n_y_temp; //Serial.print("y^2 = "); Serial.println(y_kwadrat);
      L = sqrt(z0_kwadrat + y_kwadrat); /*przekątna od servaB do punktu P(y,z)*/ //Serial.print("przekatna  [mm]= "); Serial.println(L);
      pre_alpha1_rad = (L * L + a * a - b * b) / (2 * L * a); /*obliczanie alpha1 w radianach bez acos*/ //Serial.print("pre_alpha1_rad= "); Serial.println(pre_alpha1_rad);
      pre_alpha2_rad = n_y_temp / L; /*obliczanie alpha2 w radianach bez acos*/ //Serial.print("pre_alpha2_rad= "); Serial.println(pre_alpha2_rad);
      alpha_rad = acos(pre_alpha1_rad) + acos(pre_alpha2_rad); /*cały kąt alpha w radianach*/ //Serial.print("alpha_rad= "); Serial.println(alpha_rad);
      alpha = (180 / PI) * alpha_rad; /*docelowy kąt alpha*/ if (b_show_info == true) {
        Serial.print("  |  alpha= ");
        Serial.print(alpha);
      }
      //pre_beta_rad = (a*a + b*b - L*L) / (2*a*b); /*obliczanie bety w radianach bez acos*/ //Serial.print("pre_beta_rad= "); Serial.println(pre_beta_rad);
      beta_rad = acos((a * a + b * b - L * L) / (2 * a * b)); /*obliczanie bety w radianach*/ //Serial.print("beta_rad= "); Serial.println(beta_rad);
      beta = (180 / PI) * beta_rad; /*kąt beta docelowy.*/ 
      if (b_show_info == true) 
      {
        Serial.print(", beta= ");
        Serial.print(beta);
      }
      if (beta >= 157)
      {
        Serial.print("!!!");
        beta = 156; //!!servo beta ma słaby zakres!!!
      }
      if (b_up == true || b_down == true || stringOne.substring(0, 1) == "(") //jeżeli ruch łapa idzie w dół/górę lub ruch odbywa się funkcją serwisową...
      { //... to licz kąt fi normalnie, tak by chwytak był zawsze prostopadły do planszy...
        fi = 270 - alpha - beta; /*kąt fi docelowy*/
      }
      else fi = 179; //...a jeżeli nie to podnieś chwytak najwyżej jak się da, by przy przenoszeniu bierek nie chaczył o inne.
      if (b_show_info == true)
      {
        Serial.print(", fi= ");
        if (fi == 179) Serial.print(fi);
        else Serial.print(fi + 7);
        Serial.print(", kp= "); Serial.println(n_katPodstawa);
      }

      if (alpha <= 180 && alpha >= 0 && beta <= 157 && beta >= 24 && fi <= 180 && fi >= 45) //dopuszczalne kąty.
        //servo beta dziala w przedziale od 24 do 157 stopni. servo fi minimum 45, bo by gniotło się o ramię przed nim
      { //jeżeli wartości kątów mieszczą się w dopuszczalnych wartościach, to zezwól na ich ustalenie i kontynuuj obliczanie/ruch
        servoB.write(alpha, n_servo_speed, false); PrintAngleToLCD("servoB");
        servoC.write(beta, n_servo_speed, false); PrintAngleToLCD("servoC");
        servoD.write(fi + 7, 70, false); PrintAngleToLCD("servoD"); //bardzo problematyczne jest ustawić kąt mechanicznie. dodaję +7stopni do fi zawsze by wyszło tyle ile jest założone.
      }
      else //jeżeli kąt wyskoczył poza dopuszczalne wartości, to ustaw bezpieczne wartości, pokaż gdzie był błąd w zmiennych i wyjdź z pętli
      {
        Serial.println("ERROR: Kat poza zakresem. Przerwanie ruchu i wyjscie z petli.");
        servoB.write(90, n_servo_speed, false); PrintAngleToLCD("servoB");
        servoC.write(90, n_servo_speed, false); PrintAngleToLCD("servoC");
        servoD.write(90, n_servo_speed, false); PrintAngleToLCD("servoD");
        //pokaz gdzie sie program wykrzaczyl na obliczeniach
        Serial.print("z0 = "); Serial.println(z0);
        Serial.print("z0^2 = "); Serial.println(z0_kwadrat); //pokaz gdzie sie program wykrzaczyl na obliczeniach
        Serial.print("y^2 = "); Serial.println(y_kwadrat);
        Serial.print("przekatna L [mm]= "); Serial.println(L);
        Serial.print("L^2 = "); Serial.println(L * L);
        Serial.print("pre_alpha1_rad= "); Serial.println(pre_alpha1_rad);
        Serial.print("pre_alpha2_rad= "); Serial.println(pre_alpha2_rad);
        Serial.print("alpha_rad= "); Serial.println(alpha_rad);
        Serial.print("pre_beta_rad= "); Serial.println(pre_beta_rad);
        Serial.print("beta_rad= "); Serial.println(beta_rad);
        alpha = 90; beta = 90; fi = 90; y = b; z = z1 + a; //wartości jak z 'reset'
        PrintPosToLCD("y", y); PrintPosToLCD("z", z);
        i = n_wsp_ruchu - 1; //po tym wyjdzie z petli
      }
      delay(15000 / (n_servo_speed * n_wsp_ruchu)); //po każdej iteracji czekaj chwilę, aby serwa zdążyły powoli dojechać. współczynnik wyznaczony empirycznie (tj. na oko).
      if (b_przerywanie_petli == true)
      { //dla wstepnego ustawienia ramion tylko raz licz katy serw
        b_przerywanie_petli = false;
        i = n_wsp_ruchu - 1; //te 'i' przerwie pętle
      }
    }
    //wartości mimo iż są ok, przypisywane są jeszcze raz, bo możliwe że po wielu przejściach kąty będą się przesuwać o małe wartości
    d_y_temp = y; /*powtórne dla pierwszego ustawienia ramienia- nie szkodzi*/
    d_z_temp = z;
    if (n_speed_changed == 0) n_servo_speed = SERVO_SPEED;  //predkosc serw podczas normalnej pracy (polecenia 'up' i 'down' zmieniają prędkość. tutaj trzeba to naprawiać)...
    else n_servo_speed = n_speed_changed; //...jeżeli jednak prędkość była zmieniana ręcznie to będzie ona wracała zawsze do podanej ręcznie wartości.
    if (b_up == true && b_sekwencja_ruchow == true)
    {
      servoD.write(179, 70, false); //jeżeli łapa się podniosła podczas rozgrywania partii szachów, to podnieś na maxymalnie nad bierki...
      PrintAngleToLCD("servoD");
    }
    //...poprzez danie maxymalnego kąta fi (podobnie co wyżej. dobrze by było zrobić to w kodzie w 1 bloku i miejscu)
    else if (b_down == false) //po przeniesieniu się nad planszą ustaw sobie już kąt do zejścia po bierkę
    {
      fi = 270 - alpha - beta;
      servoD.write(fi + 7, 70, false);
      PrintAngleToLCD("servoD");
    }
    b_up = false; b_down = false; //jeżeli był wykonywny ruch typu up/down to zresetuj te zmienne, by w kolejnym przejściu arduino nie zareagowało na nie.
    if (b_sekwencja_ruchow == true) //jeżeli mieliśmy do czynienia z ruchem generowanym z gry/ze strony...
    {
      if (Str_move_case == "armUp2") //... i jeżeli był to ostatni ruch z sekwencji przemieszczania pionka...
      {
        servoA.write(110, n_servo_speed, false); PrintAngleToLCD("servoA"); //... to serwem podstawy ustaw się na środku planszy ...
        b_sekwencja_ruchow = false; //... i nie wykonuj już więcej funkcji związanym z grą na stronie WWW...
      }
       //... a na końcu wyślij do core informację o tym jaki żądany ruch z core'a został wykonany:
      // jeżeli żądanie ruchu zawierało pole nad które miała się łapa ruszyć, to do informacji zwrotnej dopisz też informację o tym jakie to było pole (zbędna ochrona)...
      if (Str_move_case == "movedFrom" || Str_move_case == "movedTo" || Str_move_case == "movedToR") {
        String Str_sqare = " " + (String)Str_litera_pola + (String)n_wsp_n;
        AnswerToCore(Str_move_case,Str_sqare);
      }
      else AnswerToCore(Str_move_case,""); //...a dla innych ruchów wyślij samą odpowiedź o wykonanym ruchu.
      // !!w rdzeniu (core) mam stare ustawienia wg których każda wiadomość kończy się dolarem. zobaczyć czy to coś psuje (nie widać by coś się złego działo).
    }
    stringOne = ""; //po przejściu przez cały program wyczyść tą zmienną, by w kolejnym przejściu nie został uruchomiony któryś warunek.
    if (b_show_info == true) Serial.println("");
  }
}

void AnswerToCore(String Str_msg1, String Str_msg2)
{
  if (b_znaki_koncow_linii)
  {
    String Str_answer = "@" + Str_msg1 + Str_msg2 + "$";
    Serial.print(Str_answer);
  }
  else
  {
    Serial.print(Str_msg1);
    Serial.println(Str_msg2);
  }
}

void PrintAngleToLCD(String Str_servo)
{
  int n_row, n_column, n_kat;
  if (Str_servo == "servoA")
  {
    n_row = 0;
    n_column = 0;
    n_kat = (int) n_katPodstawa;
  }
  else if (Str_servo == "servoB")
  {
    n_row = 6;
    n_column = 0;
    n_kat = (int) alpha;
  }
  else if (Str_servo == "servoC")
  {
    n_row = 12;
    n_column = 0;
    n_kat = (int) beta;
  }
  else if (Str_servo == "servoD")
  {
    n_row = 0;
    n_column = 1;
    n_kat = (int) fi;
  }
  else if (Str_servo == "servoE")
  {
    n_row = 6;
    n_column = 1;
    n_kat = (int) n_katSzczeki;
  }
  else if (Str_servo == "servoF")
  {
    n_row = 12;
    n_column = 1;
    n_kat = (int) n_wsp_ks2 - n_katSzczeki;
  }

  lcd1.setCursor(n_row, n_column);
  lcd1.print(Str_servo.substring(5, 6));
  lcd1.setCursor(n_row + 1, n_column);
  if (n_kat >= 100) lcd1.print(n_kat);
  else //kwestie kosmetyczne na wyświetlaczu
  {
    lcd1.print("0");
    lcd1.setCursor(n_row + 2, n_column);
    if (n_kat >= 10) lcd1.print(n_kat);
    else
    {
      lcd1.print("0");
      lcd1.setCursor(n_row + 3, n_column);
      lcd1.print(n_kat);
    }
  }
}

void PrintPosToLCD(String Str_pos, int n_axis_pos)
{
  int n_row;
  if (Str_pos == "x" || Str_pos == "X")
  {
    Str_pos = "X=";
    n_row = 0;
  }
  else if (Str_pos == "y" || Str_pos == "Y")
  {
    Str_pos = "Y=";
    n_row = 6;
  }
  else if (Str_pos == "z" || Str_pos == "Z")
  {
    Str_pos = "Z=";
    n_row = 12;
  }

  lcd2.setCursor(n_row, 0);
  lcd2.print(Str_pos);
  lcd2.setCursor(n_row + 2, 0);
  if (n_axis_pos == -1) lcd2.print("???"); //jeżeli pozycja nie jest znana, bo nie jest liczona (np. podczas podawania kąta bezpośrednio na serwo), to wyświetlaj "???"
  else if (n_axis_pos >= 100) lcd2.print(n_axis_pos);
  else 
  {
    lcd2.print("0");
    lcd2.setCursor(n_row + 3, 0);
    if (n_axis_pos >= 10) lcd2.print(n_axis_pos);
    else
    {
      lcd2.print("0");
      lcd2.setCursor(n_row + 4, 0);
      lcd2.print(n_axis_pos);
    }
  }
}

