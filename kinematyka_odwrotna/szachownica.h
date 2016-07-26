#ifndef szachownica_h
#define szachownica_h

#include "Arduino.h"
#include <math.h> //cosinusy, PI itd
#include "komunikacja.h"
#include "lcd_pos.h"

class cSzachownica
{
  private:
    cKomunikacja* _pKomunikacja;
    cLCD_pos* _pLCD_pos;
    String _sLiteraPola, _sCyfraPola; //pole planszy szachownicy rozbite na 2 składowe i wrzucone do Stringów do dalszych przekształceń ( z char'ami miałem problemy).
    int _nWsp_m; //interpretacja pola planszy szachownicy (liczby). "m" może operować wartościami od 3 do -4 (ważne dla obliczeń).
    float _f_y0, _fDlugoscPola, _fBladSrodka; //na podstawie tych wartości i 4 skrajnych pól szachownicy liczone są położenia wszsytkich innych pól planszy w grze.
    //zmienne zostały zmierzone linijką, a potem metodami sztucznej inteligencji w matlabie dociągnięte do takich wartości, by ze wzoru na wszsytkie pola szachownicy najlepiej wychodziły te
    //4 krańcowe pola wzięte do obliczeń.

    double _dSkladowa_y_KataPodstawy, _dSkladowa_x_KataPodstawy; //realne wartości 'y' i 'x' potrzebne by wyliczyć poprawny kąt podstawy dla danego pola planszy (czyli wartość x). nie ma tu żadnego powiązania ze zmiennymi:
    //y, z, n_y, n_z (+ich stringi) które służą do liczenia położenia punktu P na płaszczyźnie (y,z).

    double _d_b, _d_c; //specjalne osobne 'x' i 'y' do obliczania standardowego 'y' punktu P płaszczyzny (y,z). Za pomocą wzorów na 'x' i 'kp' wyliczono 4 skrajne pola planszy
    //(w matlabie) i na podstawie tych punktów wyliczono środki reszty pól szachownicy (też w matlabie). punkty te są zinterpretowane jako 2 wektory, gdzie A1=(d_b_array[0],d_c_array[0])...
    //A8=(d_b_array[7],d_c_array[7]), B1=(d_b_array[8],d_c_array[8])... H8=(d_b_array[63],d_c_array[63]).
    //matlab, chyba wyliczane to jest na podstawie 'y' i 'x' z kp, potem SI->wsp. wzoru liczace wektory i na ich podstawie liczony wsp. y do P(y,z)

    double ObliczSkladowa_y_DlaKataPodstawy();
    double ObliczSkladowa_x_DlaKataPodstawy();


  public:
    //----------------------------------------------------------KONSTRUKTOR----------------------------------------------------------//
    cSzachownica(cKomunikacja* pKomunikacja, cLCD_pos* pLCD_pos);

    //------------------------------------------------------------METODY-------------------------------------------------------------//
    void OkreslLiterePola(String sPolecenieRdzenia);
    void OkreslCyfrePola(String sPolecenieRdzenia);
    int ObliczKatPodstawy(); // ??? obliczaniem tego powinna zająć się klasa "silnik" ???
    unsigned long Oblicz_y(); // ??? obliczaniem tego powinna zająć się klasa "ramie" ???

    //----------------------------------------------------METODY-DOSTĘPOWE-DO-PÓL----------------------------------------------------//
    String getLiteraPola() const                        { return _sLiteraPola; }
    String getCyfraPola() const                         { return _sCyfraPola; }
};

#endif
