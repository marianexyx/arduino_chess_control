//-------------------------------------------------------------FUNKCJE-------------------------------------------------------------//
void SprawdzTypPolecenia(String sKomendaRdzenia)
{
  //enum CORE_COMMAND { CC_RESET, CC_UP, CC_DOWN, CC_SPEED, CC_TURNON, CC_TURNOFF, CC_INFOON, CC_INFOOFF, CC_KP, CC_ALPHA, CC_BETA, CC_FI, CC_KS, CC_OPEN, CC_CLOSE, CC_TRASH, CC_YZ, CC_XYZ, CC_XY, CC_ELSE };
  //CORE_COMMAND KomendaRdzenia;
  if (sKomendaRdzenia.substring(0, 5) == "reset") KomendaRdzenia = CC_RESET;
  else if (sKomendaRdzenia.substring(2, 4) == "up") KomendaRdzenia = CC_UP;
  else if (sKomendaRdzenia.substring(2, 6) == "down") KomendaRdzenia = CC_DOWN;
  else if (sKomendaRdzenia.substring(0, 7) == "speed =") KomendaRdzenia = CC_SPEED;
  else if (sKomendaRdzenia.substring(0, 7) == "turn on") KomendaRdzenia = CC_TURNON;
  else if (sKomendaRdzenia.substring(0, 8) == "turn off") KomendaRdzenia = CC_TURNOFF;
  else if (sKomendaRdzenia.substring(0, 7) == "info on") KomendaRdzenia = CC_INFOON;
  else if (sKomendaRdzenia.substring(0, 8) == "info off") KomendaRdzenia = CC_INFOOFF;
  else if (sKomendaRdzenia.substring(0, 4) == "kp =") KomendaRdzenia = CC_KP;
  else if (sKomendaRdzenia.substring(0, 7) == "alpha =") KomendaRdzenia = CC_ALPHA;
  else if (sKomendaRdzenia.substring(0, 6) == "beta =") KomendaRdzenia = CC_BETA;
  else if (sKomendaRdzenia.substring(0, 4) == "fi =") KomendaRdzenia = CC_FI;
  else if (sKomendaRdzenia.substring(0, 4) == "ks =") KomendaRdzenia = CC_KS;
  else if (sKomendaRdzenia.substring(2, 6) == "open") KomendaRdzenia = CC_OPEN;
  else if (sKomendaRdzenia.substring(2, 7) == "close") KomendaRdzenia = CC_CLOSE;
  else if (sKomendaRdzenia.substring(2, 7) == "trash") KomendaRdzenia = CC_TRASH;
  else if (sKomendaRdzenia.substring(0, 1) == "(" && sCoreCommand.substring(8, 9) == ")") KomendaRdzenia = CC_YZ;
  else if (sKomendaRdzenia.substring(0, 1) == "(" && sCoreCommand.substring(11, 12) == ")") KomendaRdzenia = CC_XYZ;
  else if (sKomendaRdzenia.substring(2, 3) == "[" && sCoreCommand.substring(5, 6) == "]") KomendaRdzenia = CC_XY;
  else KomendaRdzenia = CC_ELSE;
}

float fWektorOdchylek[] = { -11, -10.9, -10.8, -10.7, -10.6, -10.5, -10.4, -10.3, -10.2, -10.1, -10, -9.7, -9.4, -9.1, -8.8, -8.5, -8.2, -7.9, -7.6, -7.3, -7, -7.3, -7.6, -7.9, -8.2,
                     -8.5, -8.8, -9.1, -9.4, -9.7, -10, -9.5, -9, -8.5, -8, -7.5, -7, -6.5, -6, -5.5, -5, -5, -5, -5, -5, -5, -5, -5, -5, -5, -5, -4.9, -4.8, -4.7, -4.6, -4.5, -4.4, -4.3, -4.2, -4.1, -4,
                     -4, -4, -4, -4, -4, -4, -4, -4, -4, -4, -3.7, -3.4, -3.1, -2.8, -2.5, -2.2, -1.9, -1.6, -1.3, -1, -0.9, -0.8, -0.7, -0.6, -0.5, -0.4, -0.3, -0.2, -0.1, 0, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6,
                     0.7, 0.8, 0.9, 1, 1.2, 1.4, 1.6, 1.8, 2, 2.2, 2.4, 2.6, 2.8, 3, 3.2, 3.4, 3.6, 3.8, 4, 4.2, 4.4, 4.6, 4.8, 5, 5.2, 5.4, 5.6, 5.8, 6, 6.2, 6.4, 6.6, 6.8, 7, 7.2, 7.4, 7.6, 7.8, 8, 8.2,
                     8.4, 8.6, 8.8, 9, 9.1, 9.2, 9.3, 9.4, 9.5, 9.6, 9.7, 9.8, 9.9, 10, 10.3, 10.6, 10.9, 11.2, 11.5, 11.8, 12.1, 12.4, 12.7, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13.5, 14, 14.5, 15,
                     15.5, 16, 16.5, 17, 17.5, 18, 18.1, 18.2, 18.3, 18.4, 18.5, 18.6, 18.7, 18.8, 18.9, 19, 19.2, 19.4, 19.6, 19.8, 20, 20.2, 20.4, 20.6, 20.8, 21, 21.4, 21.8, 22.2, 22.6, 23, 23.4, 23.8,
                     24.2, 24.6, 25, 25.3, 25.6, 25.9, 26.2, 26.5, 26.8, 27.1, 27.4, 27.7, 28, 27.9, 27.8, 27.7, 27.6, 27.5, 27.4, 27.3, 27.2, 27.1, 27, 27.9, 28.8, 29.7, 30.6, 31.5, 32.4, 33.3, 34.2, 35.1,
                     36
                   }; //wektor poprawek wysokości (oś z). wyliczony doświadczalnie (nigdy poprawki nie są idealne). wektor koryguje wartości z zakresu //!! na pewno 241 to rozmiar tej tablicy?
    //od y = 100 (1st element of array) do 340 mm (last), dlatego zawsze podczas zwracania odejmujemy wliczbę 100

void DodajDoPunktuPCzescRuchuRamienia()
{
  //do obliczeń potrzebuje integerów
  Ramie.ZwiekszYpartTempem(); LCD_pos.PrintPos("y", Ramie.getYtemp() );   //od ostatniej pozycji powoli jedź do nowej podanej
  Ramie.ZwiekszZpartTempem(); //PrintPosToLCD w już zawarte poprawce pionowej
  ServoA_kp.ZwiekszKPpartTempem(); //!!! tu ppwinno być przeliczanie na 'x' z 'kp'
  //Party (dYpart, dZpart, dKPpart) są zerami w ruchu ustawczym (pierwszym)
}

void WyrownanieTempowPoDodaniuOstatnichPartow()
{
  Ramie.setYtemp( Ramie.getY() ); LCD_pos.PrintPos("y", Ramie.getYtemp() );
  Ramie.setZtemp( Ramie.getZ() ); //PrintPosToLCD w poprawce pionowej
  ServoA_kp.setKPtemp( ServoA_kp.getKat() ); //!!! tu ppwinno być przeliczanie na 'x' z 'kp'

  Komunikaty.PokazInfo(INFO_ODLEGLOSC_Y, (String)Ramie.getYtemp() );
  Komunikaty.PokazInfo(INFO_ODLEGLOSC_Z, (String)Ramie.getZtemp() );
  Komunikaty.PokazInfo(INFO_KAT_KP, (String)ServoA_kp.getKPtemp() );
}

void ObliczPartyPrzemieszczenia()
{
  if (Ramie.getYtemp() == 0 || Ramie.getZtemp() == 0 || ServoA_kp.getKPtemp() == 0) //jeżeli nie było jeszcze żadnego przejścia przez pętle ruchu (tj. nie ma zapisnego ruchu poprzedniego) to tempy są zerami
  {
    Ramie.setYtemp(Ramie.getY() ); LCD_pos.PrintPos("y", Ramie.getY() ); //tempy odpowiadają za pamiętanie poprzednio osiąganych 'y' i 'z'...
    Ramie.setYtemp(Ramie.getZ() ); LCD_pos.PrintPos("z", Ramie.getZ() );  //...dzięki temu program zna drogę ramienia skąd-dokąd
    ServoA_kp.setKPtemp(ServoA_kp.getKat() ); LCD_pos.PrintPos("kp", ServoA_kp.getKat() );
    bPrzerwaniePetli = true; //dzięki tej zmiennej po złapaniu pozycji pierwszej i zarazem docelowej przy pierwszym ruchu program nie przechodzi bez sensu przez całą pętle ruchu, tylko...
    //...przerywa ją po 1 przejściu.
  }
  else  //dla każdego normalnego podanego ruchu oblicz części ruchu do dodawania w pętli ruchu
  {
    Ramie.setYpart((Ramie.getY() - Ramie.getYtemp() ) / nWspRuchu ); // floaty. minusowe wartosci dla zmiennej, jezeli punkt jest "z tyłu" (tzn. nowa wartość osi 'y', 'z', 'kp' jest mniejsza od poprzedniej).
    Ramie.setZpart((Ramie.getZ() - Ramie.getZtemp() ) / nWspRuchu );
    ServoA_kp.setKPtemp((ServoA_kp.getKat() - ServoA_kp.getKPtemp() ) / nWspRuchu );
  }
}

void OgraniczenieWysokosci()
{
  if (Ramie.getZtemp() >= 236) //dla najdalszych bierek maxymalne możliwe podniesienie ramienia to 236 (dalej jest to poza polem roboczym manipulatora). powinno to być liczone w...
  { //...zalezności od możliwości, ale póki co jest to zablokowane dla wszystkich pozycji.
    Ramie.setZtemp(235); LCD_pos.PrintPos("z", Ramie.getZtemp() );
    //nServoSpeed = 255; //osiągnij prędkośc dla serw maxymalną, by szybko wyjść z warunku blokującego ruch (w tym przypadku serwa się nie ruszają, więc prędkość maxymalna...
    //...serw jest tylko zmienną obliczeniową by w ułamku sekundy wyjśc z funkcji delay) - coś się tu pierdoli
  }
  /*  else // !!!! trochę naokoło ten kod poniżej. można się go pozbyć jak to się zrobi mądrzej
    {
      if (nSpeedChanged == 0) nServoSpeed = SERVO_SPEED;  //predkosc serw podczas normalnej pracy (polecenia 'up' i 'down' zmieniają prędkość. tutaj trzeba to naprawiać)...
      else nServoSpeed = nSpeedChanged;
      if (Ramie.getUpState() == true || Ramie.getDownState() == true) nServoSpeed = MOTOR_SPEED_SLOW;
    }*/
}

int WartoscServaPozaDopuszczalnymZakresem()
{
  ServoA_kp.UstawKat(90); LCD_angle.PrintAngle("ServoA_kp", ServoA_kp.getKat() );
  ServoB_alpha.UstawKat(90); LCD_angle.PrintAngle("ServoB_alpha", ServoB_alpha.getKat() );
  ServoC_beta.UstawKat(90); LCD_angle.PrintAngle("ServoC_beta", ServoC_beta.getKat() );
  ServoD_fi.UstawKat(90); LCD_angle.PrintAngle("ServoD_fi", ServoD_fi.getKat() );

  Komunikaty.ServoPozaZakresem();

  Ramie.setY(Ramie.getDlugoscB()); Ramie.setZ(Ramie.getDlugoscZ1() + Ramie.getDlugoscA() ); //wartości jak z 'reset'
  LCD_pos.PrintPos("y", Ramie.getY()); LCD_pos.PrintPos("z", Ramie.getZ() );
  return nWspRuchu - 1; //po tym wyjdź z petli
}

void Reset()
{
  Komunikaty.OdpowiedzNaRdzen("reset");
  Ramie.setY(Ramie.getDlugoscB() );
  Ramie.setZ(Ramie.getDlugoscZ1() + Ramie.getDlugoscA() ); LCD_pos.PrintPos("y", Ramie.getY() ); LCD_pos.PrintPos("z", Ramie.getZ() ); //ustawienia wynikajace z katow = 90
  sCoreCommand = "(184,296)"; //to samo co wyzej
  Serwis.ZmienPredkosc(); //bez podania 2go parametru predkosc sie zresetuje na MOTOR_SPEED_NORMAL (18)
  ServoA_kp.UstawKat(90); LCD_angle.PrintAngle("ServoA_kp", ServoA_kp.getKat() ); LCD_pos.PrintPos("x", -1);
  ServoE_ks1.UstawKat(90); LCD_angle.PrintAngle("ServoE_ks1", ServoE_ks1.getKat() );
  ServoF_ks2.UstawKat(200 - 90); LCD_angle.PrintAngle("ServoF_ks2", ServoF_ks2.getKat() );
}

int PrzerwijPetle()
{
  bPrzerwaniePetli = false;
  return nWspRuchu - 1; //owa zmienna 'i' przerwie pętle w kolejnym przejściu
}

void PrewencyjnieDociagnijDoZadanegoPolozenia()
{
  ServoA_kp.setKPtemp(ServoA_kp.getKat() ); /*powtórne dla pierwszego ustawienia ramienia- nie szkodzi*/
  Ramie.setYtemp(Ramie.getY() );
  Ramie.setZtemp(Ramie.getZ() );
}

void UstawKatySerw()
{
  ServoA_kp.UstawKat(ServoA_kp.getKPtemp()); /*ServoA_kp.write(n_kp_temp, nServoSpeed, false);*/ //tutaj to nie zadziala. musi być warunek dla n_kp_temp (co?)

  ServoB_alpha.UstawKat(ServoB_alpha.ObliczKatAlfa(Ramie.getPrzekatnaRamieniaL(), Ramie.getDlugoscA(), Ramie.getDlugoscB(), Ramie.getYtemp()));
  LCD_angle.PrintAngle("ServoB_alpha", ServoB_alpha.getKat() );

  ServoC_beta.UstawKat(ServoC_beta.ObliczKatBeta(Ramie.getPrzekatnaRamieniaL(), Ramie.getDlugoscA(), Ramie.getDlugoscB(), &Komunikaty));
  LCD_angle.PrintAngle("ServoC_beta", ServoC_beta.getKat() );

  //bardzo problematyczne jest ustawić kąt mechanicznie. zmieniam o n_fi_poprawka stopni do fi zawsze by wyszło tyle ile jest założone.
  ServoD_fi.ObliczKatFi(&Ramie, &ServoB_alpha, &ServoC_beta); LCD_angle.PrintAngle("ServoD_fi", ServoD_fi.getKat() ); //!!!z jakis powodow tutaj predkosc ustawiona była na 35
}

void WykonajRuchRamieniem()
{
  ObliczPartyPrzemieszczenia(); //dla każdego normalnego podanego ruchu oblicz części ruchu do dodawania w pętli ruchu
  for (int i = 0; i < nWspRuchu; i++) //wykonuj cząstkowe ruchy "nWspRuchu" razy, aż ruch się wykona, lub do błędnych kątów serw. duży warunek.
  {
    DodajDoPunktuPCzescRuchuRamienia();
    if (i >= nWspRuchu - 1) WyrownanieTempowPoDodaniuOstatnichPartow(); //docelowy zadany punkt. jeśli gdzieś powstałyby minimalne błędy, to tu są naprawiane.
    //poniżej poprawka wysokosci - niech program myśli że jego zadane wartości są idealnie odwzorywane, a realnie obliczaj i ustawiaj kąty tak, by wyszło lepiej o skompensowany zmierzony błąd
    Ramie.setZtemp(Ramie.getZtemp() + fWektorOdchylek[(int)Ramie.getYtemp()]);
    LCD_pos.PrintPos("z", Ramie.getZtemp() ); //dodaj różnicę wysokości podstawy chwytaka i planszy
    Komunikaty.PokazInfo(INFO_ODLEGLOSC_Z_UPGR, (String)Ramie.getZtemp() ); //poprawkę wysokości jako metoda ramienia
    OgraniczenieWysokosci();

    if (ServoB_alpha.getKat() <= 180 && ServoB_alpha.getKat() >= 0 && ServoC_beta.getKat() <= 157 && ServoC_beta.getKat() >= 24 && ServoD_fi.getKat() <= 180 && ServoD_fi.getKat() >= 45) UstawKatySerw(); // !!! dopuszczalne kąty.
    else i = WartoscServaPozaDopuszczalnymZakresem(); //jeżeli kąt wyskoczył poza dopuszczalne wartości, to ustaw bezpieczne wartości, pokaż gdzie był błąd w zmiennych i wyjdź z pętli

    delay(15000 / (Ramie.getPredkosc() * nWspRuchu)); //po każdej iteracji czekaj chwilę, aby serwa zdążyły powoli dojechać. współczynnik wyznaczony empirycznie (tj. na oko).
    if (bPrzerwaniePetli == true) i = PrzerwijPetle(); //wyjscie z petli for w szczegolnych wypadkach- error lub
  }

  PrewencyjnieDociagnijDoZadanegoPolozenia(); //wartości mimo iż są ok, przypisywane są jeszcze raz, bo możliwe że po wielu przejściach kąty będą się przesuwać o małe wartości
  //UstawFiDoKolejnegoRuchu(&Ramie, &ServoB_alpha, &ServoA_kp);
  if (Ramie.getDownState() == false && Komunikaty.InfoDlaRdzenia() != "r_trashed")  ServoB_alpha.PodniesPrewencyjnie(); //jeżeli funkcja nie wykonywała ruchu typu down ani usuwania bierki to podnies prewecyjnie, aby nie wywracało bierek skrajnych

  if (Komunikaty.getSekwencjaRuchow() == true) //jeżeli mieliśmy do czynienia z ruchem generowanym z gry/ze strony
  {
    if (Komunikaty.InfoDlaRdzenia() == "armUp2") //jeżeli był to ostatni ruch z sekwencji przemieszczania pionka
    {
      ServoA_kp.UstawKat(110); LCD_angle.PrintAngle("ServoA_kp", ServoA_kp.getKat() );  //to serwem podstawy ustaw się na środku planszy
      Komunikaty.setSekwencjaRuchow(false); // nie wykonuj już więcej funkcji związanym z grą na stronie WWW, aż do Stringa rozpoczynającego sekwencje ruchów
    }
    Komunikaty.OdpowiedzNaRdzen(Komunikaty.InfoDlaRdzenia(), ""); //wyślij do core informację o tym jaki żądany ruch z core'a został wykonany
  }

  /*sprzatanie i przywracanie zmiennych*/
  if (Serwis.getPredkoscSerwisowa() == 0) Ramie.setPredkosc(MOTOR_SPEED_NORMAL); //jeżei serwisowo nie była zmieniana prędkosc, to ustaw predkosc serw na normalną (moglo to byc zmieniane przez polecenia up/down)...
  else Ramie.setPredkosc(Serwis.getPredkoscSerwisowa() ); //...jeżeli jednak prędkość była zmieniana ręcznie to będzie ona musiala wracac zawsze do tej podanej wartości.
  Ramie.setUpState(false); Ramie.setDownState(false); //jeżeli był wykonywny ruch typu up/down to zresetuj te zmienne, by w kolejnym przejściu arduino nie zareagowało na nie.
  sCoreCommand = ""; //po przejściu przez cały program wyczyść tą zmienną, by w kolejnym przejściu nie został uruchomiony któryś warunek.
}

