#ifndef ramie_h
#define ramie_h

#include "Arduino.h"
#include <VarSpeedServo.h>
#include "silnik.h"


class cRamie: public cSilnik
{
  private:
    double _dX, _dY, _dZ; //trójwymiar
    double _dKp, _dAlpha, _dBeta, _dFi, _dKs; //kąty w ramienia
    double _dL;  //przekątna od servaB do punktu P(y,z). wartość obliczana dopiero jak podane jest "z" w "P".

  public:
    cRamie();

    double ObliczPrzekatnaRamieniaL();

    double getPrzekatnaRamieniaL() const         { return _dL; }
/*
    //ruchy standardowe ramienia
    up()
    down()
    [a1]()
    trash()
    pierwszy ruch
    
    kolejne przejścia
    oblicz składowe przestrzeni
    ograniczenie wysokości

    odstaw do wyłączenia
    */
};

#endif
