#ifndef funkcje_h
#define funkcje_h

class class_name
{
  public:
    class_name(); //konstruktor
    // destruktor?
    void SETUP(); //definicja metody (funkcji)
};

extern class_name funkcje; //nazwa która będzie używana w głównym skeczu (odwoływanie się do klasy jak do obiektu? 
                        //albo raczej stworzenie obiektu "funkcje" klasy "fclass" (bez słowa new? co to był extern?)
                        //-extern pozwala widzieć na zewnątrz pliku (ponadglobalne zmienne- unikać)

#endif
