#include "Arduino.h"
#include "komunikacja.h"

cKomunikacja::cKomunikacja(cServoA* pServoA, cServoB* pServoB, cServoC* pServoC, cServoD* pServoD, cServoEF* pServoEF, cRamie* pRamie)
{
  _bPokazujeInfo = false;
  _bZnakiKoncowLinii = true; //definiuje czy dodajemy w funkcji znaki dla początku i końca wiadomości dla poprawnej komunikacji asynchronicznej z core'm
  _sInfoDlaRdzenia = "none";
  _bSekwencjaRuchow = false;

  _pServoA = pServoA;
  _pServoB = pServoB;
  _pServoC = pServoC;
  _pServoD = pServoD;
  _pSilnikE = pServoE;
  _pSilnikF = pServoF;
  _pRamie = pRamie;
}

void cKomunikacja::PokazPozycjeRamienia()
{
  if (_bPokazujeInfo == true)
  {
    Serial.print("kp = "); Serial.print(_pServoA.getKat());
    Serial.print(", alpha= "); Serial.print(_pServoB.getKat());
    Serial.print(", beta= "); Serial.print(_pServoC.getKat()); if ((int)_pServoC.getKat() >= 157) Serial.print("!!!");
    Serial.print(", fi= "); Serial.println(_pServoD.getKat());
    Serial.print(" | y = "); Serial.print(_pRamie.getYtemp()); //Ramie.getYtemp() + Z???
    Serial.print(", z = "); Serial.print(_pRamie.getZtemp());   //wysokość będzie podawana ta, którą dopiero chcemy realnie osiągnąć poprzez kompensację błędu chwilę dalej, a którą zadaliśmy
    Serial.print(", z_upgr = "); Serial.print(_pRamie.getZtemp()); if ((int)dZmiennaDoPokazania >= 236) Serial.print("!!!");
  }
}

void cKomunikacja::PokazInfo(double dZmiennaDoPokazania, INFO_TYPE TypInformacji)
{
  if (_bPokazujeInfo)
  {
    switch (TypInformacji)
    {
      case INFO_FIELD_LETTER:
        Serial.print("litera pola = ");
        if (dZmiennaDoPokazania == 3) Serial.println("a");
        else if ( dZmiennaDoPokazania == 2) Serial.println("b");
        else if ( dZmiennaDoPokazania == 1) Serial.println("c");
        else if ( dZmiennaDoPokazania == 0) Serial.println("d");
        else if ( dZmiennaDoPokazania == -1) Serial.println("e");
        else if ( dZmiennaDoPokazania == -2) Serial.println("f");
        else if ( dZmiennaDoPokazania == -3) Serial.println("g");
        else if ( dZmiennaDoPokazania == -4) Serial.println("h");
        else Serial.println("ERROR: Błędne pole planszy.");
        break;
      case INFO_FIELD_NUMBER:
        Serial.print("cyfra pola = ");
        Serial.println(dZmiennaDoPokazania);
        break;
      /*case INFO_KAT_KP:
        Serial.print("kp = ");
        Serial.print((int)dZmiennaDoPokazania);
        break;
        case INFO_KAT_ALPHA:
        Serial.print(", alpha= ");
        Serial.print(dZmiennaDoPokazania);
        break;
        case INFO_KAT_BETA:
        Serial.print(", beta= ");
        Serial.print(dZmiennaDoPokazania);
        if ((int)dZmiennaDoPokazania >= 157) Serial.print("!!!");
        break;
        case INFO_KAT_FI:
        Serial.print(", fi= ");
        Serial.println(dZmiennaDoPokazania);
        break;
        case INFO_ODLEGLOSC_Y:
        Serial.print(" | y = ");
        Serial.print(dZmiennaDoPokazania);
        break;
        case INFO_ODLEGLOSC_Z:
        //wysokość będzie podawana ta, którą dopiero chcemy realnie osiągnąć
        //poprzez kompensację błędu chwilę dalej, a którą zadaliśmy
        Serial.print(", z = ");
        Serial.print(dZmiennaDoPokazania);
        break;
        case INFO_ODLEGLOSC_Z_UPGR:
        Serial.print(", z_upgr = ");
        Serial.print(dZmiennaDoPokazania);
        if ((int)dZmiennaDoPokazania >= 236) Serial.print("!!!");
        break;*/
      case INFO_OPEN:
        Serial.print("opened. katSzczeki = ");
        Serial.println(dZmiennaDoPokazania);
        break;
      case INFO_CLOSE:
        Serial.print("closed. katSzczeki = ");
        Serial.println(dZmiennaDoPokazania);
        break;
      case INFO_UP:
        Serial.print("up: z[mm] = ");
        Serial.println(dZmiennaDoPokazania);
        break;
      case INFO_DOWN:
        Serial.print("down: z[mm] = ");
        Serial.println(dZmiennaDoPokazania);
        break;
      default:
        Serial.print("error: wrong var TypInformacji (INFO_TYPE) in cKomunikacja::PokazInfo()");
        Serial.println(dZmiennaDoPokazania);
        break;
    }
  }
  return;
}

void cKomunikacja::PrzygotujOdpowiedzNaRdzen(String sPolecenieRdzenia)
{
  String sTypRuchuBierki = "";
  if (sPolecenieRdzenia.substring(0, 2) == "n_") sTypRuchuBierki = "n_"; //zostaje ustawiona zmienna, która po wykonaniu ruchu przez
  //łapę zostanie wysłana core, jako potwierdzenie tego co miało się wykonać (i czekanie na kolejny sygnał z sekwencji ruchów
  else if (sPolecenieRdzenia.substring(0, 2) == "r_") //sekwencja poprzedza zwykłe przemieszczanie bierki. ma to miejsce, gdy bierka ma wylądować na polu na którym
    //jest bierka wroga. o tym czy na bitym polu na pewno jest bierka wroga decyduje program szachowy, który przed ruchami arduino bada czy ruch może być wykonany.
  {
    this->PokazujInfo(false); //na wszelki wypadek wyłącz pokazywanie innych informacji (rozwaliły by komunikację)
    this->setSekwencjaRuchow(true); //rozpoczęto rozmowe z core'm, więc niektóre ruchy wykonywać mają się trochę inaczej
    sTypRuchuBierki = "r_";
  }
  else if (sPolecenieRdzenia.substring(0, 2) == "c_") sTypRuchuBierki = "c_";

  String sTypRuchuRamienia = "";
  if (sPolecenieRdzenia.substring(2, 2) == "up") sTypRuchuRamienia = "armUp";
  else if (sPolecenieRdzenia.substring(2, 6) == "down") sTypRuchuRamienia = "armDown";
  else if (sPolecenieRdzenia.substring(2, 6) == "open") sTypRuchuRamienia = "opened";
  else if (sPolecenieRdzenia.substring(2, 7) == "close") sTypRuchuRamienia = "closed";
  else if (sPolecenieRdzenia.substring(5, 7) == "]f")//jeżeli wiadomość to np. [a2]f to arduino rozpoczyna wymianę danych z core'm
    //wiadomość oznacza żądanie przeniesienia pionka "z pozycji (f-from)". w tym ruchu ustawiane jest ramie w pukncie P nad pionkiem do podniesienia.
  {
    this->PokazujInfo(false); //na wszelki wypadek wyłącz pokazywanie innych informacji (rozwaliły by komunikację)
    this->setSekwencjaRuchow(true); //rozpoczęto rozmowe z core'm, więc niektóre ruchy wykonywać mają się trochę inaczej
    sTypRuchuRamienia = "movedFrom";
  }
  else if (sPolecenieRdzenia.substring(5, 7) == "]t") sTypRuchuRamienia = "movedTo";
  else if (sPolecenieRdzenia.substring(2, 7) == "trash") sTypRuchuRamienia = "trashed";

  String sKtoryTakiRuch = "";
  String sOstatniZnak = sPolecenieRdzenia.substring(sPolecenieRdzenia.length() - 1, 1);
  if (sOstatniZnak == "1" || sOstatniZnak == "2" || sOstatniZnak == "3" ||
      sOstatniZnak == "4" || sOstatniZnak == "K" || sOstatniZnak == "R") sKtoryTakiRuch = sOstatniZnak;
  else sKtoryTakiRuch = "";

  _sInfoDlaRdzenia = sTypRuchuBierki + sTypRuchuRamienia + sKtoryTakiRuch;
}

void cKomunikacja::OdpowiedzNaRdzen(String sMsgPart) // ! moge z tego zrobic jeda funkcje z tą z dołu
{
  if (_bZnakiKoncowLinii == true)
  {
    String sAnswer = "@" + sMsgPart + "$";
    Serial.print(sAnswer);
  }
  else
  {
    Serial.println(sMsgPart);
  }
}

void cKomunikacja::OdpowiedzNaRdzen(String sMsgPart1, String sMsgPart2)
{
  String sAnswer;
  Serial.println("testy w OdpowiedzNaRdzen");
  if (_bZnakiKoncowLinii == true)
  {
    sAnswer = "@" + sMsgPart1 + sMsgPart2 + "$";
    Serial.print(sAnswer);
  }
  else
  {
    sAnswer = sMsgPart1 + sMsgPart2;
    Serial.println(sAnswer);
  }
}

void cKomunikacja::ServoPozaZakresem()
{
  // !! wysrac full wiadomosci jezeli mamy doczynenia z funkcja serwisowa
  /*//pokaz gdzie sie program wykrzaczyl na obliczeniach
    Serial.print("z0 = "); Serial.println(z0);
    Serial.print("z0^2 = "); Serial.println(z0_kwadrat); //pokaz gdzie sie program wykrzaczyl na obliczeniach
    Serial.print("y^2 = "); Serial.println(y_kwadrat);
    Serial.print("przekatna L [mm]= "); Serial.println(L);
    Serial.print("L^2 = "); Serial.println(L * L);
    Serial.print("pre_alpha1_rad= "); Serial.println(pre_alpha1_rad);
    Serial.print("pre_alpha2_rad= "); Serial.println(pre_alpha2_rad);
    Serial.print("alpha_rad= "); Serial.println(alpha_rad);
    Serial.print("pre_beta_rad= "); Serial.println(pre_beta_rad);
    Serial.print("beta_rad= "); Serial.println(beta_rad);*/ //teog troche mi się nie chce. musiałbym się uczyć wrzucać cały obiekt w argumenty innego obiektu. może kiedyś
  Serial.println("Error: Servo poza zakresem dopuszczalnych kątów");
}

