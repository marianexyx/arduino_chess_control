#include "Arduino.h"
#include "lcd_pos.h"

void cLCD_pos::PrintPos(String sPosition, int nAxisPosition)
{
  if (sPosition == "x" || sPosition == "X") { sPosition = "X="; _nRow = 0; }
  else if (sPosition == "y" || sPosition == "Y") { sPosition = "Y="; _nRow = 6; }
  else if (sPosition == "z" || sPosition == "Z") { sPosition = "Z="; _nRow = 12; }
  
  _pLCD_pos->setCursor(_nRow, 0);
  _pLCD_pos->print(sPosition);
  _pLCD_pos->setCursor(_nRow + 2, 0);
  if (nAxisPosition == -1) _pLCD_pos->print("???"); // jeżeli pozycja nie jest znana, bo nie jest liczona (np. podczas 
                                                 // podawania kąta bezpośrednio na serwo), to wyświetlaj "???"
  else if (nAxisPosition >= 100) _pLCD_pos->print(nAxisPosition);
  else
  {
    _pLCD_pos->print("0");
    _pLCD_pos->setCursor(_nRow + 3, 0);
    if (nAxisPosition >= 10) _pLCD_pos->print(nAxisPosition);
    else
    {
      _pLCD_pos->print("0");
      _pLCD_pos->setCursor(_nRow + 4, 0);
      _pLCD_pos->print(nAxisPosition);
    }
  }
}

