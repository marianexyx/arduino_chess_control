#ifndef ramie_h
#define ramie_h

#pragma once

#include "Arduino.h"
#include <VarSpeedServo.h>
#include "silnik.h"
#include "komunikacja.h"
#include "lcd_angle.h"

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
    double getZupgr() const                    { return _dZtemp + _fWektorOdchylek[(int)_dYtemp]; }
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
