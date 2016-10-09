#ifndef ramie_h
#define ramie_h

#pragma once

#include "Arduino.h"
#include <VarSpeedServo.h>
#include "silnik.h"
#include "komunikacja.h"
#include "lcd_angle.h"

/*float fWektorOdchylek[] = { -11, -10.9, -10.8, -10.7, -10.6, -10.5, -10.4, -10.3, -10.2, -10.1, -10, -9.7, -9.4, -9.1, -8.8, -8.5, -8.2, -7.9, -7.6, -7.3, -7, -7.3, -7.6, -7.9, -8.2,
                            -8.5, -8.8, -9.1, -9.4, -9.7, -10, -9.5, -9, -8.5, -8, -7.5, -7, -6.5, -6, -5.5, -5, -5, -5, -5, -5, -5, -5, -5, -5, -5, -5, -4.9, -4.8, -4.7, -4.6, -4.5, -4.4, -4.3, -4.2, -4.1, -4,
                            -4, -4, -4, -4, -4, -4, -4, -4, -4, -4, -3.7, -3.4, -3.1, -2.8, -2.5, -2.2, -1.9, -1.6, -1.3, -1, -0.9, -0.8, -0.7, -0.6, -0.5, -0.4, -0.3, -0.2, -0.1, 0, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6,
                            0.7, 0.8, 0.9, 1, 1.2, 1.4, 1.6, 1.8, 2, 2.2, 2.4, 2.6, 2.8, 3, 3.2, 3.4, 3.6, 3.8, 4, 4.2, 4.4, 4.6, 4.8, 5, 5.2, 5.4, 5.6, 5.8, 6, 6.2, 6.4, 6.6, 6.8, 7, 7.2, 7.4, 7.6, 7.8, 8, 8.2,
                            8.4, 8.6, 8.8, 9, 9.1, 9.2, 9.3, 9.4, 9.5, 9.6, 9.7, 9.8, 9.9, 10, 10.3, 10.6, 10.9, 11.2, 11.5, 11.8, 12.1, 12.4, 12.7, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13.5, 14, 14.5, 15,
                            15.5, 16, 16.5, 17, 17.5, 18, 18.1, 18.2, 18.3, 18.4, 18.5, 18.6, 18.7, 18.8, 18.9, 19, 19.2, 19.4, 19.6, 19.8, 20, 20.2, 20.4, 20.6, 20.8, 21, 21.4, 21.8, 22.2, 22.6, 23, 23.4, 23.8,
                            24.2, 24.6, 25, 25.3, 25.6, 25.9, 26.2, 26.5, 26.8, 27.1, 27.4, 27.7, 28, 27.9, 27.8, 27.7, 27.6, 27.5, 27.4, 27.3, 27.2, 27.1, 27, 27.9, 28.8, 29.7, 30.6, 31.5, 32.4, 33.3, 34.2, 35.1,
                            36
                          }; //wektor poprawek wysokości (oś z). wyliczony doświadczalnie (nigdy poprawki nie są idealne). wektor koryguje wartości z zakresu //!! na pewno 241 to rozmiar tej tablicy?
//od y = 100 (1st element of array) do 340 mm (last), dlatego zawsze podczas zwracania odejmujemy wliczbę 100*/

//class cSilnik;

class cRamie: public cSilnik
{
  private:
    cKomunikacja* _pKomunikacja;
    cLCD_angle* _pLCD_angle;
    cSilnik* _pSilnikE; 
    cSilnik* _pSilnikF;
    double _dX, _dY, _dZ; //trójwymiar !!! były to jako zmienne unsigned long
    /*uwaga: wartośći y, z (czy n_y, n_z) są tak naprawdę współczynnikami pozycji punktu P, a nie realnymi wartościami tego punktu w milimetrach w przestrzeni rzeczywistej (wartości
    liczone od podstawy ramienia). nie sprawdziłem tych wartości przy liczeniu kinematyki odwrotnej i odbiegają one o jakieś 10% od tego co się da obliczyć linijką. dlatego później w
    programie występują zmienne typu: d_kp_y, d_kp_x, d_b, d_c (oraz cały blok zmiennych potrzebnych do liczenia ich) na podstawie których liczony jest kąt podstawy i jest komparacja
    położenia pól planszy szachownicy w stosunku do punktu P(y,z) liczonego na współczynnikach.*/
    int _nZ1; //[mm] odległość planszy od servaB (alpha) - pierwszy człon ruchumy ramienia wisi w powietrzu
    double _dKs; //kąty w ramienia
    static double _dL;  //przekątna od servaB do punktu P(y,z). wartość obliczana dopiero jak podane jest "z" w "P".
    double _dA, _dB, _dC; //dlugosci ramion [mm]. dlugosc C mierzona do podstawy chwytaka. reszta to odległości pomiędzy orczykami serw
    double _dYpart, _dZpart; //zwiększane kawałki od aktualnego punktu P do nowopodanego punktu P (w iteracjach)
    double _dYtemp, _dZtemp; //tymczasowy y,z do obliczen (używane przy d_y_part, d_z_part)
    bool _bUp, _bDown; //zmienne informują czy w danych ruchu ramienia było ono podnoszone, czy opuszczane (ważne dla prostowania serwa kąta fi, by nic nie wywrócić).
    int _nKatSzczeki; //środek zakresu pracy serwa
    const float _fWektorOdchylek[241]; //stały wektor poprawek, będzie w liscie inicjalizacyjenj

  public:
    //----------------------------------------------------------KONSTRUKTOR----------------------------------------------------------//
    cRamie(cKomunikacja* pKomunikacja, cLCD_angle* pLCD_angle, cSilnik* pSilnikE, cSilnik* pSilnikF);

    //------------------------------------------------------------METODY-------------------------------------------------------------//
    void ObliczPrzekatnaRamieniaL();
    void Open();
    void Close();
    void Up();
    void Down();
    void UstawKatSzczeki(int nKatSzczek);
    void ZwiekszYpartTempem()                {_dYtemp += _dYpart; }
    void ZwiekszZpartTempem()                {_dZtemp += _dZpart; }
    
    //----------------------------------------------------METODY-DOSTĘPOWE-DO-PÓL----------------------------------------------------//
    double getPrzekatnaRamieniaL() const       { return cRamie::_dL; }
    double getDlugoscA() const                 { return _dA; }
    double getDlugoscB() const                 { return _dB; }
    double getDlugoscC() const                 { return _dC; }
    double getX() const                        { return _dX; }
    double getY() const                        { return _dY; }
    double getZ() const                        { return _dZ; }
    double getZupgr() const                    { return _dZ + _fWektorOdchylek[(int)_dYtemp]; }
    int getDlugoscZ1() const                   { return _nZ1; }
    double getYtemp() const                    { return _dYtemp; } 
    double getYpart() const                    { return _dYpart; } 
    double getZtemp() const                    { return _dZtemp; } 
    double getZpart() const                    { return _dZpart; } 
    bool getUpState() const                    { return _bUp; }
    bool getDownState() const                  { return _bDown; }   
    int getKatSzczeki() const                  { return _nKatSzczeki; }   

    void setX(double dX)                       { _dX = dX; }
    void setY(double dY)                       { _dY = dY; }
    void setZ(double dZ)                       { _dZ = dZ < 150 ? 150 : dZ; } //(condition) ? (if_true) : (if_false)
    void setYtemp(double dYtemp)               { _dYtemp = dYtemp; }
    void setZtemp(double dZtemp)               { _dZtemp = dZtemp; }
    void setYpart(double dYpart)               { _dYpart = dYpart; }
    void setZpart(double dZpart)               { _dZpart = dZpart; }
    void setUpState(bool bUp)                  { _bUp = bUp; }
    void setDownState(bool bDown)              { _bDown = bDown; }
    
    /*
    odstaw do wyłączenia
    */
};

#endif
