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

/*String inDoubleOutString(double dIncDouble, int nDlugoscStrOut, int nDlugoscPoPrzecinku)
{
  Serial.println("try funckje.cpp: inDoubleOutString()");
  char a_p_chVal[15]; //stworz tablice char'ów
  a_p_chVal[0] = (char)0; //powinno wyczyścić tablicę char'ów                                                         
  String sVal = "";  //stworz i wyzeruj wyjsciowy string
  dtostrf(dIncDouble, nDlugoscStrOut, nDlugoscStrOut, a_p_chVal); //zamień double na wskaźnik na tablicę char'ów
  for (int i = 0; i < sizeof(a_p_chVal); i++) sVal += a_p_chVal[i]; //zamień wskaźnik na tablicę char'ów na string
  Serial.println("done funckje.cpp: inDoubleOutString()");
  return sVal; //zwróć stringa
}*/

void DodajDoPunktuPCzescRuchuRamienia()
{
  //do obliczeń potrzebuje integerów
  Ramie.ZwiekszYpartTempem(); LCD_pos.PrintPos("y", Ramie.getYtemp() );   //od ostatniej pozycji powoli jedź do nowej podanej
  Ramie.ZwiekszZpartTempem(); //PrintPosToLCD w już zawarte poprawce pionowej
  ServoA_kp.ZwiekszKPpartTempem(); //!!! tu ppwinno być przeliczanie na 'x' z 'kp'
  //Party (dYpart, dZpart, dKPpart) są zerami w ruchu ustawczym (pierwszym)
  //Serial.println("done funckje.cpp: DodajDoPunktuPCzescRuchuRamienia()");
}

void WyrownanieTempowPoDodaniuOstatnichPartow()
{
  Ramie.setYtemp( Ramie.getY() ); LCD_pos.PrintPos("y", Ramie.getYtemp() );
  Ramie.setZtemp( Ramie.getZ() ); //PrintPosToLCD w poprawce pionowej
  ServoA_kp.setKPtemp( ServoA_kp.getKat() ); //!!! tu ppwinno być przeliczanie na 'x' z 'kp'

 /* Komunikaty.PokazInfo(Ramie.getYtemp(), INFO_ODLEGLOSC_Y);
  Komunikaty.PokazInfo(Ramie.getZtemp(), INFO_ODLEGLOSC_Z);
  Komunikaty.PokazInfo(ServoA_kp.getKPtemp(), INFO_KAT_KP);*/
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
    Ramie.setZtemp(235);
    //LCD_pos.PrintPos("z", Ramie.getZtemp() ); //cała ta funkcja z jakiegos powodu nie chce ruszyc, nawet jak daje niby normalne wartosci
    //Serial.println("done funckje.cpp: OgraniczenieWysokosci(): if function: LCD_pos.PrintPos('z', Ramie.getZtemp()");
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
  //Serial.println("try funckje.cpp: ServoA_kp.setKPtemp(ServoA_kp.getKat() );");
  ServoA_kp.setKPtemp(ServoA_kp.getKat() ); /*powtórne dla pierwszego ustawienia ramienia- nie szkodzi*/
  //Serial.println("try funckje.cpp: Ramie.setYtemp(Ramie.getY() ); Ramie.setZtemp(Ramie.getZ() );");
  Ramie.setYtemp(Ramie.getY() );
  Ramie.setZtemp(Ramie.getZ() );
  //Serial.println("done funckje.cpp: PrewencyjnieDociagnijDoZadanegoPolozenia()");
}

void UstawKatySerw()
{
  Ramie.ObliczPrzekatnaRamieniaL();

  //Serial.println("try funckje.cpp: ServoA_kp.UstawKat(ServoA_kp.getKPtemp())");
  ServoA_kp.UstawKat(90.0f);
  /*ServoA_kp*///ServoB_alpha.UstawKat(90.0d /*ServoA_kp.getKPtemp()*/); /*ServoA_kp.write(n_kp_temp, nServoSpeed, false);*/ //tutaj to nie zadziala. musi być warunek dla n_kp_temp (co?)

  //Serial.println("try funckje.cpp: ServoB_alpha.UstawKat(ServoB_alpha.ObliczKatAlfa(Ramie.getPrzekatnaRamieniaL(), Ramie.getDlugoscA(), Ramie.getDlugoscB(), Ramie.getYtemp()));");
  ServoB_alpha.UstawKat(ServoB_alpha.ObliczKatAlfa(Ramie.getPrzekatnaRamieniaL(), Ramie.getDlugoscA(), Ramie.getDlugoscB(), Ramie.getYtemp()));
  LCD_angle.PrintAngle("ServoB_alpha", ServoB_alpha.getKat() );

  //Serial.println("try funckje.cpp: ServoC_beta.UstawKat(ServoC_beta.ObliczKatBeta(Ramie.getPrzekatnaRamieniaL(), Ramie.getDlugoscA(), Ramie.getDlugoscB(), &Komunikaty));");
  ServoC_beta.UstawKat(ServoC_beta.ObliczKatBeta(Ramie.getPrzekatnaRamieniaL(), Ramie.getDlugoscA(), Ramie.getDlugoscB(), &Komunikaty));
  LCD_angle.PrintAngle("ServoC_beta", ServoC_beta.getKat() );

  //Serial.println("try funckje.cpp: ServoD_fi.ObliczKatFi(&Ramie, &ServoB_alpha, &ServoC_beta); ");
  //bardzo problematyczne jest ustawić kąt mechanicznie. zmieniam o n_fi_poprawka stopni do fi zawsze by wyszło tyle ile jest założone.
  ServoD_fi.ObliczKatFi(&Ramie, &ServoB_alpha, &ServoC_beta);
  LCD_angle.PrintAngle("ServoD_fi", ServoD_fi.getKat() ); //!!!z jakis powodow tutaj predkosc ustawiona była na 35
}

void WykonajRuchRamieniem()
{
  ObliczPartyPrzemieszczenia(); //dla każdego podanego ruchu ramienia oblicz najmniejsze części ruchu jakie to ramię będzie wykonywało aż do zadanego punktu
  for (int i = 0; i < nWspRuchu; i++) //wykonuj cząstkowe ruchy "nWspRuchu" razy, aż ruch się wykona, lub do błędnych kątów serw. duży warunek.
  {
    DodajDoPunktuPCzescRuchuRamienia(); //zwiększ aktualną pozycję ramienia o najmniejszą cząstkę ruchu minimalnie je przesuwając do zadanego punktu końcowego
    if (i >= nWspRuchu - 1) WyrownanieTempowPoDodaniuOstatnichPartow(); //pośrednia funckja naprawiająca: ostatna dodana cząsteczka ruchu, tj. docelowy zadany punkt. jeśli gdzieś powstałyby minimalne błędy, to tu są naprawiane.
    //poniżej poprawka wysokosci - niech program myśli że jego zadane wartości są idealnie odwzorywane, a realnie obliczaj i ustawiaj kąty tak, by wyszło lepiej o skompensowany zmierzony błąd
    Ramie.setZtemp(Ramie.getZupgr());
    LCD_pos.PrintPos("z", Ramie.getZupgr() ); //dodaj różnicę wysokości podstawy chwytaka i planszy
    //Komunikaty.PokazInfo(Ramie.getZupgr(), INFO_ODLEGLOSC_Z_UPGR); 
    OgraniczenieWysokosci(); //zanim obliczone wartości zostaną ustawione na silnikach sprawdź, czy nie wyszły poza dopuszczalne wartości. jeżeli wyszły to je w tej funkcji ogranicz

    if (ServoB_alpha.getKat() <= 180 && ServoB_alpha.getKat() >= 0 && ServoC_beta.getKat() <= 157 && ServoC_beta.getKat() >= 24 && ServoD_fi.getKat() <= 180 && ServoD_fi.getKat() >= 45) 
    {
      UstawKatySerw();
    }
    else 
    {
      //jeżeli kąt wyskoczył poza dopuszczalne wartości, to ustaw bezpieczne wartości, pokaż gdzie był błąd w zmiennych i wyjdź z pętli
      i = WartoscServaPozaDopuszczalnymZakresem();
    }

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

