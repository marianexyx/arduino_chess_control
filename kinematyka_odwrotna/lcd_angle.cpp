#include "Arduino.h"
#include "lcd_angle.h"

void cLCD_angle::PrintAngle(String sServo, float fKat) //rozwiązanie przy założeniu że zawsze pilnuję jaki kąt wrzucam
{
  if (sServo == "ServoA_kp") {
    _nRow = 0;
    _nColumn = 0;
  }
  else if (sServo == "ServoB_alpha") {
    _nRow = 6;
    _nColumn = 0;
  }
  else if (sServo == "ServoC_beta") {
    _nRow = 12;
    _nColumn = 0;
  }
  else if (sServo == "ServoD_fi") {
    _nRow = 0;
    _nColumn = 1;
  }
  else if (sServo == "ServoE_ks1") {
    _nRow = 6;
    _nColumn = 1;
  }
  else if (sServo == "ServoF_ks2") {
    _nRow = 12;
    _nColumn = 1;
  }
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
