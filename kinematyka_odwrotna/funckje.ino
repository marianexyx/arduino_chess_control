//-------------------------------------------------------------FUNKCJE-------------------------------------------------------------//
void DodajDoPunktuPCzescRuchuRamienia()
{
  //do obliczeń potrzebuje integerów
  Ramie.ZwiekszYpartTempem(); LCD_pos.PrintPos("y", Ramie.getYtemp() );   //od ostatniej pozycji powoli jedź do nowej podanej
  Ramie.ZwiekszZpartTempem(); //PrintPosToLCD w już zawarte poprawce pionowej
  servoA.ZwiekszKPpartTempem(); //!!! tu ppwinno być przeliczanie na 'x' z 'kp'
  //Party (dYpart, dZpart, dKPpart) są zerami w ruchu ustawczym (pierwszym)
}

void WyrownanieTempowPoDodaniuOstatnichPartow()
{
  if (i >= n_wsp_ruchu - 1) //docelowy zadany punkt. jeśli gdzieś powstałyby minimalne błędy, to tu są naprawiane
  { //... w sumie nie pamiętam co miał na celu ten warunek. pewnie jest już zbędny.
    Ramie.setYtemp( Ramie.getY() ); LCD_pos.PrintPos("y", Ramie.getYtemp() );
    Ramie.setZtemp( Ramie.getZ() ); //PrintPosToLCD w poprawce pionowej
    servoA.setKPtemp( servoA.getKat() ); //!!! tu ppwinno być przeliczanie na 'x' z 'kp'

    Komunikaty.PokazInfo(INFO_ODLEGLOSC_Y, (String)Ramie.getYtemp() );
    Komunikaty.PokazInfo(INFO_ODLEGLOSC_Z, (String)Ramie.getZtemp() );
    Komunikaty.PokazInfo(INFO_KAT_KP, (String)servoA.getKPtemp() );
  }
}

void ObliczPartyPrzemieszczenia()
{
  if (Ramie.getYtemp() == 0 || Ramie.getZtemp() == 0 || servoA.getKPtemp() == 0) //jeżeli nie było jeszcze żadnego przejścia przez pętle ruchu (tj. nie ma zapisnego ruchu poprzedniego) to tempy są zerami
  {
    Ramie.setYtemp(Ramie.getY() ); LCD_pos.PrintPos("y", Ramie.getY() ); //tempy odpowiadają za pamiętanie poprzednio osiąganych 'y' i 'z'...
    Ramie.setYtemp(Ramie.getZ() ); LCD_pos.PrintPos("z", Ramie.getZ() );  //...dzięki temu program zna drogę ramienia skąd-dokąd
    servoA.setKPtemp(servoA.getKat() ); LCD_pos.PrintPos("kp", servoA.getKat() );
    bPrzerwaniePetli = true; //dzięki tej zmiennej po złapaniu pozycji pierwszej i zarazem docelowej przy pierwszym ruchu program nie przechodzi bez sensu przez całą pętle ruchu, tylko...
    //...przerywa ją po 1 przejściu.
  }
  else  //dla każdego normalnego podanego ruchu oblicz części ruchu do dodawania w pętli ruchu
  {
    Ramie.setYpart ( (Ramie.getY() - Ramie.getYtemp() ) / nWspRuchu ); // floaty. minusowe wartosci dla zmiennej, jezeli punkt jest "z tyłu" (tzn. nowa wartość osi 'y', 'z', 'kp' jest mniejsza od poprzedniej).
    Ramie.setZpart = ( (Ramie.getZ() - Ramie.getZtemp() ) / nWspRuchu );
    servoA.setKPtemp ( (servoA.getKat() - servoA.getKPtemp() ) / nWspRuchu );
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
  else // !!!! trochę naokoło ten kod poniżej. można się go pozbyć jak to się zrobi mądrzej
  {
    if (nSpeedChanged == 0) nServoSpeed = SERVO_SPEED;  //predkosc serw podczas normalnej pracy (polecenia 'up' i 'down' zmieniają prędkość. tutaj trzeba to naprawiać)...
    else nServoSpeed = nSpeedChanged;
    if (Ramie.getUpState() == true || Ramie.getDownState() == true) nServoSpeed = MOTOR_SPEED_SLOW;
  }
}

void WartoscServaPozaDopuszczalnymZakresem()
{
  servoA.UstawKat(90, static_cast<MOTOR_SPEED>(nServoSpeed) ); LCD_angle.PrintAngle("servoA", servoA.getKat() );
  servoB.UstawKat(90, static_cast<MOTOR_SPEED>(nServoSpeed) ); LCD_angle.PrintAngle("servoB", servoB.getKat() );
  servoC.UstawKat(90, static_cast<MOTOR_SPEED>(nServoSpeed) ); LCD_angle.PrintAngle("servoC", servoC.getKat() );
  servoD.UstawKat(90, static_cast<MOTOR_SPEED>(nServoSpeed) ); LCD_angle.PrintAngle("servoD", servoD.getKat() );

  Komunikaty.ServoPozaZakresem();

  Ramie.setY(Ramie.getDlugoscB()); Ramie.setZ(Ramie.getDlugoscZ1() + Ramie.getDlugoscA() ); //wartości jak z 'reset'
  LCD_pos.PrintPos("y", Ramie.getY()); LCD_pos.PrintPos("z", Ramie.getZ() );
  i = nWspRuchu - 1; //po tym wyjdź z petli
}

void Reset()
{
  Komunikaty.OdpowiedzNaRdzen("reset");
  Ramie.setY(Ramie.getDlugoscB() );
  Ramie.setZ(Ramie.getDlugoscZ1() + Ramie.getDlugoscA() ); LCD_pos.PrintPos("y", Ramie.getY() ); LCD_pos.PrintPos("z", Ramie.getZ() ); //ustawienia wynikajace z katow = 90
  sCoreCommand = "(184,296)"; //to samo co wyzej
  servoA.UstawKat(90, MOTOR_SPEED_NORMAL); LCD_angle.PrintAngle("servoA", servoA.getKat() ); LCD_pos.PrintPos("x", -1);
  servoE.UstawKat(90, MOTOR_SPEED_NORMAL); LCD_angle.PrintAngle("servoE", servoE.getKat() );
  servoF.UstawKat(200 - 90, MOTOR_SPEED_NORMAL); LCD_angle.PrintAngle("servoF", servoF.getKat() );
}

void PrzerwijPetle()
{
  bPrzerwaniePetli = false;
  i = nWspRuchu - 1; //owa zmienna 'i' przerwie pętle w kolejnym przejściu
}

void PrewencyjnieDociagnijDoZadanegoPolozenia()
{
  servoA.setKPtemp(servoA.getKat() ); /*powtórne dla pierwszego ustawienia ramienia- nie szkodzi*/
  Ramie.setYtemp(Ramie.getY() );
  Ramie.setZtemp(Ramie.getZ() );
}

void UstawKatySerw()
{
  servoA.UstawKat(servoA.getKPtemp(), static_cast<MOTOR_SPEED>(nServoSpeed)); /*servoA.write(n_kp_temp, nServoSpeed, false);*/ //tutaj to nie zadziala. musi być warunek dla n_kp_temp
  servoB.UstawKat(ServoB_alpha.ObliczKatAlfa(Ramie.getPrzekatnaRamieniaL(), Ramie.getDlugoscA(), Ramie.getDlugoscB(), Ramie.getYtemp() ), static_cast<MOTOR_SPEED>(nServoSpeed) );
  LCD_angle.PrintAngle("servoB", ServoB_alpha.getKat() );
  servoC.UstawKat(Ramie.ObliczKatBeta(), static_cast<MOTOR_SPEED>(nServoSpeed) );
  LCD_angle.PrintAngle("servoC", servoC.getKat() );
  //bardzo problematyczne jest ustawić kąt mechanicznie. zmieniam o n_fi_poprawka stopni do fi zawsze by wyszło tyle ile jest założone.
  servoD.UstawKat(Ramie.ObliczKatFi() + Ramie.getFiPoprawka(), static_cast<MOTOR_SPEED>(nServoSpeed)); LCD_angle.PrintAngle("servoD", servoD.getKat() ); //!!!z jakis powodow tutaj predkosc ustawiona jest na 35
}

