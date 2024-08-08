#include "Buzzer.h"

const int DEFAULT_DURATION = 500; // Duração padrão em milissegundos
const int PAUSE_DURATION = 500;   // Duração da pausa entre os toques

// Método para tocar o buzzer uma vez
void Buzzer::Ring()
{
    Ring(1, DEFAULT_DURATION);
}

// Método para tocar o buzzer várias vezes com a duração padrão
void Buzzer::Ring(int count)
{
    Ring(count, DEFAULT_DURATION);
}

// Método para tocar o buzzer várias vezes com uma duração específica
void Buzzer::Ring(int count, int duration)
{
    for (byte i = 0; i < count; i++)
    {
        digitalWrite(BUZZER_BUS, HIGH); // Ativa o buzzer
        delay(duration);                // Mantém o buzzer ativo por 'duration' milissegundos
        digitalWrite(BUZZER_BUS, LOW);  // Desativa o buzzer
        if (i < count - 1)              // Evita a última pausa desnecessária
        {
            delay(PAUSE_DURATION);      // Pausa entre os toques
        }
    }
}