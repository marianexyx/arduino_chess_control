#ifndef silnik_h
#define silnik_h

#include "Arduino.h"
#include <VarSpeedServo.h>
#include <LiquidCrystal.h>
#include "lcd_angle.h"
#include "lcd_pos.h"

enum MOTOR_SPEED { MOTOR_SPEED_SLOW  = 2, MOTOR_SPEED_NORMAL= 18, MOTOR_SPEED_FAST  = 70, MOTOR_SPEED_MAX   = 255 };

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
    cSilnik(int nPin, String sNazwaServa, String sNazwaKata, float fKatMin, float fKatMax, cLCD_angle* pLCD_angle, cLCD_pos* pLCD_pos);
    // -----------------------------------------------------

    // metody
    void rozpocznij();
    void ustaw_kat(float fKat);  //podstawowy ruch - predkosc normalna, automatyczny false
    void ustaw_kat(float fKat, MOTOR_SPEED predkoscSilnika);    //podstawowy ruch - automatyczny false
    void ustaw_kat(float fKat, MOTOR_SPEED predkoscSilnika, bool bCzekajNaKoniecRuchu);  //podstawowy ruch - w domysle do wrzucania true
    void podniesPrewencyjnie(bool bOpuszczono, String sCoreAnswer, int nAlpha);  //prewencyjne odsunięcie od planszy
    void przygotujFi(bool bPozycjaGorna, bool bPozycjaDolna, bool bSekwencjaRuchow, double dFi, int nFiPoprawka, double dAlpha, double dBeta); //prepare fi motor
    void otworz();  //open 
    void zamknij(); //close 
    void smietnik();  //trash 
    //------------------------------------------------------
    
    // metody dostępowe do pól
    float Kat() const                           { return _fKat; }
    int Predkosc() const                        { return _nPredkosc; }
    String NazwaKata() const                    { return _sNazwaKata; }
    int KatMin() const                          { return _fKatMin; }
    int KatMax() const                          { return _fKatMax; }

    void Predkosc(MOTOR_SPEED predkoscSilnika)  { _nPredkosc = predkoscSilnika; } //trzeba będzie static_cast'owac?
};


#endif
