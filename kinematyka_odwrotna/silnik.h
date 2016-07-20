/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//cała ta klasa to straszliwa lipa. powinna być ona klasą wirtualną, a każdy silnik powinien być osobną klasą z przesłanianiem metod pod 
//siebie, a nie tak jak teraz osobnym obiektem. nie da rady zrobić tego na arduino ide, jako że nie zmieszczę się w pasku zakładek. póki 
//tego nie zmienię, to tak to musi być
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef silnik_h
#define silnik_h

#include "Arduino.h"
#include <VarSpeedServo.h>
#include <LiquidCrystal.h>
#include "lcd_angle.h"
#include "lcd_pos.h"

enum MOTOR_SPEED { MOTOR_SPEED_SLOW  = 2, MOTOR_SPEED_NORMAL = 18, MOTOR_SPEED_FAST  = 70, MOTOR_SPEED_MAX   = 255 };

class cLCD_angle;

class cSilnik
{
  private:
    int _nPin;
    String _sNazwaServa;
    String _sNazwaKata;
    int _nPredkosc;
    float _fKat; 
    float _fKatMin;
    float _fKatMax;
    VarSpeedServo _Servo;
    cLCD_angle* _pLCD_angle; //ustanowienie relacji zawierania z obiektem lcd
    cLCD_pos* _pLCD_pos;
    
 public:
    // konstruktory
    cSilnik() {} //dla dziedziczenia
    cSilnik(int nPin, String sNazwaServa, String sNazwaKata, float fKatMin, float fKatMax, cLCD_angle* pLCD_angle, cLCD_pos* pLCD_pos);
    // -----------------------------------------------------

    // metody
    void Rozpocznij();
    void UstawKat(float fKat);  //podstawowy ruch - predkosc normalna, automatyczny false
    void UstawKat(float fKat, MOTOR_SPEED predkoscSilnika);    //podstawowy ruch - automatyczny false
    void UstawKat(float fKat, MOTOR_SPEED predkoscSilnika, bool bCzekajNaKoniecRuchu);  //podstawowy ruch - w domysle do wrzucania true
    void PodniesPrewencyjnie(bool bOpuszczono, String sCoreAnswer, int nAlpha);  //prewencyjne odsunięcie od planszy
    void PrzygotujFi(bool bPozycjaGorna, bool bPozycjaDolna, bool bSekwencjaRuchow, double dFi, int nFiPoprawka, double dAlpha, double dBeta); //prepare fi motor
    void UstawKatSerwisowo(String sNazwaKata, String sKat); // serwisowealpha beta ks fi   
    int ObliczKatAlfa();
    int ObliczKatBeta();
    int ObliczKatFi();
    //------------------------------------------------------
    
    // metody dostępowe do pól
    float Kat() const                           { return _fKat; }
    int Predkosc() const                        { return _nPredkosc; }
    String NazwaKata() const                    { return _sNazwaKata; }
    int KatMin() const                          { return _fKatMin; }
    int KatMax() const                          { return _fKatMax; }

    void setPredkosc(MOTOR_SPEED predkoscSilnika)  { _nPredkosc = predkoscSilnika; } 
};

#endif
