#ifndef Buzzer_h
#define Buzzer_h

#include <Arduino.h>

class Buzzer
{
public:
  // Toca o buzzer uma vez com a duração padrão
  static void Ring();

  // Toca o buzzer 'count' vezes com a duração padrão
  static void Ring(int count);

  // Toca o buzzer 'count' vezes com a duração especificada em 'duration' milissegundos
  static void Ring(int count, int duration);
};

#endif
