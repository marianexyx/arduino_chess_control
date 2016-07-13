#ifndef lcd_pos_h
#define lcd_pos_h

#include "Arduino.h"
#include <LiquidCrystal.h> //panele lcd

class cLCD_pos
{
  private:
    int _nRow;
    LiquidCrystal* _pLCD_pos;

  public:
    // konstruktory
    cLCD_pos()  {}
    cLCD_pos(int nRS_pin, int nEnable_pin, int nD4_pin, int nD5_pin, int nD6_pin, int nD7_pin, int nLcdWidth, int nLcdHeight)  
    {
      _pLCD_pos = new LiquidCrystal(nRS_pin, nEnable_pin, nD4_pin, nD5_pin, nD6_pin, nD7_pin);
      _pLCD_pos->begin(nLcdWidth, nLcdHeight);
      _pLCD_pos->clear();
    }
    //-----------------------------------------------------

    // metody
    void PrintPos(String sPosition, int nAxisPosition);
    //------------------------------------------------------
    
    // metody dostępowe do pól
};

#endif
