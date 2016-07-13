#ifndef lcd_angle_h
#define lcd_angle_h

#include "Arduino.h"
#include <LiquidCrystal.h> //panele lcd

class cLCD_angle
{
  private:
    LiquidCrystal* _pLCD_angle;
    int _nRow;
    int _nColumn;
    int _nKat;
  
  public:
    // konstruktory
    cLCD_angle()  {}
    cLCD_angle(int nRS_pin, int nEnable_pin, int nD4_pin, int nD5_pin, int nD6_pin, int nD7_pin, int nLcdWidth, int nLcdHeigh)  
    {
      _pLCD_angle = new LiquidCrystal(nRS_pin, nEnable_pin, nD4_pin, nD5_pin, nD6_pin, nD7_pin);
      _pLCD_angle->begin(nLcdWidth, nLcdHeigh);
      _pLCD_angle->clear();
    }
    //-----------------------------------------------------

    // metody
    void PrintAngle(String sServo, float fKat); //kąt trzeba wrzucić metodą dostępową obiektu od klasy silnika
    //------------------------------------------------------
    
    // metody dostępowe do pól
};

#endif
