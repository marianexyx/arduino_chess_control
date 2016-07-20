#include "Arduino.h"
#include "szachownica.h"

cSzachownica::cSzachownica(cKomunikacja* pKomunikacja, cLCD_pos* pLCD_pos)
{
  _pKomunikacja = pKomunikacja;
  _pLCD_pos = pLCD_pos;
  _f_y0 = 114;
  _fDlugoscPola = 28.6;
  _fBladSrodka = 8.8;
  _d_b = 0;
  _d_c = 0;

}

void cSzachownica::OkreslLiterePola(String sPolecenieRdzenia)
{
  if (sPolecenieRdzenia.substring(11, 12) == ")") _sLiteraPola = sPolecenieRdzenia.substring(9, 10); //sprawdź jaka podana jest litera pola dla funkcji serwisowych...
  else if (sPolecenieRdzenia.substring(5, 6) == "]") _sLiteraPola = sPolecenieRdzenia.substring(3, 4); //... i funkcji obsługi gry.
  else Serial.print("error1");

  _pKomunikacja->PokazInfo(INFO_FIELD_LETTER, (String)_sLiteraPola);

  if (_sLiteraPola == "a" || _sLiteraPola == "A") {
    _nWsp_m = 3;  //zamień tą literę na współczynnik n_wsp_m (jego wartośc jest ważna dla obliczeń)...
    _sLiteraPola = "a";
  }
  else if (_sLiteraPola == "b" || _sLiteraPola == "B") {
    _nWsp_m = 2;  //...oraz dla ułatwienia jeżeli podano format wielkich liter, to zamień na małe.
    _sLiteraPola = "b";
  }
  else if (_sLiteraPola == "c" || _sLiteraPola == "C") {
    _nWsp_m = 1;
    _sLiteraPola = "c";
  }
  else if (_sLiteraPola == "d" || _sLiteraPola == "D") {
    _nWsp_m = 0;  //wartości n_wsp_m wynikają z tego, że podstawa ramienia jest mniej więcej...
    _sLiteraPola = "d";
  }
  else if (_sLiteraPola == "e" || _sLiteraPola == "E") {
    _nWsp_m = -1;  //...na środku planszy i odległość na osi 'x' liczymy z udziałem mnożnika...
    _sLiteraPola = "e";
  }
  else if (_sLiteraPola == "f" || _sLiteraPola == "F") {
    _nWsp_m = -2;  //...pól w jedną lub drugą stronę. prawie te same obliczenia są w pliku...
    _sLiteraPola = "f";
  }
  else if (_sLiteraPola == "g" || _sLiteraPola == "G") {
    _nWsp_m = -3;  //...matlabowskim (załaczonym)
    _sLiteraPola = "g";
  }
  else if (_sLiteraPola == "h" || _sLiteraPola == "H") {
    _nWsp_m = -4;
    _sLiteraPola = "h";
  }
  else Serial.println("ERROR: Złe podane pole planszy.");
}

void cSzachownica::OkreslCyfrePola(String sPolecenieRdzenia)
{
  if (sPolecenieRdzenia.substring(11, 12) == ")") _sCyfraPola = sPolecenieRdzenia.substring(10, 11).toInt(); //sprawdź jaka podana jest cyfra pola dla funkcji serwisowych...
  else if (sPolecenieRdzenia.substring(5, 6) == "]") _sCyfraPola = sPolecenieRdzenia.substring(4, 5).toInt(); //...i dla funkcji obsługi gry. bezpośrednie przypisanie
  else Serial.print("error2");

  _pKomunikacja->PokazInfo(INFO_FIELD_NUMBER, (String)_sCyfraPola);
}

double cSzachownica::ObliczSkladowa_y_DlaKataPodstawy()
{
  return _dSkladowa_y_KataPodstawy = _f_y0 + (_fDlugoscPola * _fBladSrodka); //z rzeczywistych zależności oblicz składową 'y' dla obliczenia kąta podstawy
}

double cSzachownica::ObliczSkladowa_x_DlaKataPodstawy() //oblicz z rzeczywistych zależności składową 'x' dla obliczenia kąta podstawy
{
  if (_nWsp_m >= 0) //dla pól A,B,C,D
    _dSkladowa_x_KataPodstawy = _fDlugoscPola * _nWsp_m + (_fBladSrodka + _fDlugoscPola / 2);
  else //dla pól E,F,G,H
    _dSkladowa_x_KataPodstawy  = _fDlugoscPola * (-_nWsp_m - 1) + _fDlugoscPola / 2 - _fBladSrodka; //wzór jest tutaj trochę inny, bo są "z drugiej strony planszy"

  _pLCD_pos->PrintPos("x", _dSkladowa_x_KataPodstawy);

  return _dSkladowa_x_KataPodstawy;
}

int cSzachownica::ObliczKatPodstawy()  //oblicz szukany kąt podstawy z magicznych matematycznych zależności. // ??? obliczaniem tego powinna zająć się klasa "silnik" ???
{
  _dSkladowa_y_KataPodstawy = ObliczSkladowa_y_DlaKataPodstawy();
  _dSkladowa_x_KataPodstawy = ObliczSkladowa_x_DlaKataPodstawy();

  int nKatPodstawa;

  if (_nWsp_m >= 0) //dla pól A,B,C,D
    nKatPodstawa = round((180 / PI) * atan(_dSkladowa_y_KataPodstawy / _dSkladowa_x_KataPodstawy)); //zaokrąglamy ręcznie, bo i tak to się stanie w funkcji write(), a tu funkcja round() zrobi to lepiej (na zasadzie ceil/floor).
  else //dla pól E,F,G,H. wzór jest tutaj trochę inny- wpływa na to zasięg kątów w tangensie
    nKatPodstawa = round(180 - ((180 / PI) * atan(_dSkladowa_y_KataPodstawy / _dSkladowa_x_KataPodstawy)));

  return nKatPodstawa;
}

unsigned long cSzachownica::Oblicz_y() // ??? obliczaniem tego powinna zająć się klasa "ramie" ???
{
  //współczynniki do poniższego wzoru obliczone podobnie jak te przy wzorze na 'kp'. najpierw trzeba było znaleść wzór określający wszsytkie środki pól na...
  //...planszy, współczynniki znalezione w matlabie metodami SI za pomocą 4 skrajnych pól planszy.

  //pozycje 4 skrajnych środków pól szachownicy (dociągane matematycznie by dało się określić wzór który byłby w stanie wyznaczyć te 4 zmierzone punkty)
  double x_a1 = 112.4; double y_a1 = 107.4;
  double x_a8 = 318; double y_a8 = 110.2;
  double x_h1 = 110.5; double y_h1 = -88.5;
  double x_h8 = 317.3; double y_h8 = -92.2;

  //odległości pomiędzy skrajnymi polami (ich środkami) szachownicy
  double x_a1_a8 = x_a8 - x_a1; double y_a1_a8 = y_a8 - y_a1;
  double x_a1_h1 = x_a1 - x_h1; double y_a1_h1 = y_a1 - y_h1;
  double x_h1_h8 = x_h8 - x_h1; double y_h1_h8 = y_h8 - y_h1;
  double x_a8_h8 = x_a8 - x_h8; double y_a8_h8 = y_a8 - y_h8;

  double ogolny_x = x_a1 + (_sCyfraPola.toInt() * (x_a1_a8/7 + ((_nWsp_m/14) * (x_a1_h1/7 - x_a8_h8/7)))) - (_nWsp_m * (x_a1_h1/7 - ((_sCyfraPola.toInt()/14) * (x_h1_h8/7 - x_a1_a8/7)))); //składowa pionowa
  double ogolny_y = y_a1 + (_sCyfraPola.toInt() * (y_a1_a8/7 + ((_nWsp_m/14) * (y_a1_h1/7 - y_a8_h8/7)))) - (_nWsp_m * (y_a1_h1/7 - ((_sCyfraPola.toInt()/14) * (y_h1_h8/7 - y_a1_a8/7)))); //składowa pozioma
  
  unsigned long y = sqrt(pow(ogolny_x,2) + pow(ogolny_y,2)); //wartość której szukamy- przekątna dwóch składowych powyżej

  return y;
}

