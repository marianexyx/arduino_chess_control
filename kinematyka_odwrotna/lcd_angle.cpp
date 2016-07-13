#include "Arduino.h"
#include "lcd_angle.h"

void cLCD_angle::PrintAngle(String sServo, float fKat) //rozwiązanie przy założeniu że zawsze pilnuję jaki kąt wrzucam
{
  if (sServo == "servoA") {_nRow = 0; _nColumn = 0; }
  else if (sServo == "servoB") { _nRow = 6; _nColumn = 0; }
  else if (sServo == "servoC") { _nRow = 12; _nColumn = 0; }
  else if (sServo == "servoD") { _nRow = 0; _nColumn = 1; }
  else if (sServo == "servoE") { _nRow = 6; _nColumn = 1; }
  else if (sServo == "servoF") { _nRow = 12; _nColumn = 1; }
  _nKat = (int) fKat;

  _pLCD_angle->setCursor(_nRow, _nColumn);
  _pLCD_angle->print(sServo.substring(5, 6));
  _pLCD_angle->setCursor(_nRow + 1, _nColumn);
  if (_nKat >= 100) _pLCD_angle->print(_nKat);
  else //kwestie kosmetyczne na wyświetlaczu
  {
    _pLCD_angle->print("0");
    _pLCD_angle->setCursor(_nRow + 2, _nColumn);
    if (_nKat >= 10) _pLCD_angle->print(_nKat);
    else
    {
      _pLCD_angle->print("0");
      _pLCD_angle->setCursor(_nRow + 3, _nColumn);
      _pLCD_angle->print(_nKat);
    }
  }
  
  /*int n_row, n_column, n_kat;
  if (Str_servo == "servoA")
  {
    n_row = 0;
    n_column = 0;
    n_kat = (int) n_katPodstawa;
  }
  else if (Str_servo == "servoB")
  {
    n_row = 6;
    n_column = 0;
    n_kat = (int) alpha;
  }
  else if (Str_servo == "servoC")
  {
    n_row = 12;
    n_column = 0;
    n_kat = (int) beta;
  }
  else if (Str_servo == "servoD")
  {
    n_row = 0;
    n_column = 1;
    n_kat = (int) fi;
  }
  else if (Str_servo == "servoE")
  {
    n_row = 6;
    n_column = 1;
    n_kat = (int) n_katSzczeki;
  }
  else if (Str_servo == "servoF")
  {
    n_row = 12;
    n_column = 1;
    n_kat = (int) n_wsp_ks2 - n_katSzczeki;
  }

  lcd1.setCursor(n_row, n_column);
  lcd1.print(Str_servo.substring(5, 6));
  lcd1.setCursor(n_row + 1, n_column);
  if (n_kat >= 100) lcd1.print(n_kat);
  else //kwestie kosmetyczne na wyświetlaczu
  {
    lcd1.print("0");
    lcd1.setCursor(n_row + 2, n_column);
    if (n_kat >= 10) lcd1.print(n_kat);
    else
    {
      lcd1.print("0");
      lcd1.setCursor(n_row + 3, n_column);
      lcd1.print(n_kat);
    }
  }*/
}

//cLCD_angle LCD_angle = cLCD_angle(); //wymagane dla kompilatora. ????
