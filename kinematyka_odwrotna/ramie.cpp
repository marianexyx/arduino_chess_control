#include "Arduino.h"
#include <VarSpeedServo.h>
#include "silnik.h"
#include "ramie.h"

cRamie::cRamie()
{
  ;
}

double cRamie::PrzekatnaRamieniaL(int n_y_temp, int n_z_temp)
{
  unsigned long ul_y_kwadrat, ul_z0_kwadrat; //druga potega y i z0 (wartości inaczej się nie mieszczą w obliczeniach- wypadają poza zasięgi zmiennych, dlatego są liczone osobno)
  unsigned long ul_z0; //odległość punktu P(y,z) od wysokośći na której jest servoB w pionie (wartość obliczana dopiero jak podane jest "z" w "P").
  int n_z1 = 116; //[mm] odległość planszy od servaB (alpha) - pierwszy człon ruchumy ramienia wisi w powietrzu
  
  ul_y_kwadrat = n_y_temp * n_y_temp;
  ul_z0 = n_z_temp - n_z1; //odległość punktu P(y,z) od wysokośći na której jest servoB w pionie- założenie, że punkt nigdy nie jest poniżej serva (nigdy nie będzie)
  ul_z0_kwadrat = ul_z00 * ul_z0;
  return _dL = sqrt(ul_z0_kwadrat + ul_y_kwadrat); //przekątna od servaB do punktu P(y,z)
}

