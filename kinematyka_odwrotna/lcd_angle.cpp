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
}
