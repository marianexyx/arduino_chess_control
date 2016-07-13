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
  
  /*int n_row;
  if (Str_pos == "x" || Str_pos == "X")
  {
    Str_pos = "X=";
    n_row = 0;
  }
  else if (Str_pos == "y" || Str_pos == "Y")
  {
    Str_pos = "Y=";
    n_row = 6;
  }
  else if (Str_pos == "z" || Str_pos == "Z")
  {
    Str_pos = "Z=";
    n_row = 12;
  }

  lcd2.setCursor(n_row, 0);
  lcd2.print(Str_pos);
  lcd2.setCursor(n_row + 2, 0);
  if (n_axis_pos == -1) lcd2.print("???"); //jeżeli pozycja nie jest znana, bo nie jest liczona (np. podczas podawania kąta bezpośrednio na serwo), to wyświetlaj "???"
  else if (n_axis_pos >= 100) lcd2.print(n_axis_pos);
  else
  {
    lcd2.print("0");
    lcd2.setCursor(n_row + 3, 0);
    if (n_axis_pos >= 10) lcd2.print(n_axis_pos);
    else
    {
      lcd2.print("0");
      lcd2.setCursor(n_row + 4, 0);
      lcd2.print(n_axis_pos);
    }
  }*/
}

//cLCD_pos LCD_pos = cLCD_pos(); //wymagane dla kompilatora. ????
