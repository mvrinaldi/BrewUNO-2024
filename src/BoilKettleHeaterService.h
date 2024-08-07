#ifndef BoilKettleHeaterService_h
#define BoilKettleHeaterService_h

#include "HeaterService.h"

class BoilKettleHeaterService : public HeaterService
{
public:
  // Construtor para inicializar o serviço do aquecedor do caldeirão de fervura
  BoilKettleHeaterService(TemperatureService *temperatureService, ActiveStatus *activeStatus, BrewSettingsService *brewSettingsService);
  void StartPID(double kp, double ki, double kd);

protected:
  void SetUP(); // Método para configurar o serviço
  bool StopCompute(); // Método para verificar se deve parar o cálculo do PID
  PID &GetPid(); // Método para obter uma referência ao PI
  void PidCompute(); // Método para calcular o PID
  double GetPidOutput(); // Método para obter a saída do PID
  double GetPidInput(); // Método para obter a entrada do PID
  double GetPidSetPoint(); // Método para obter o setpoint do PID
  uint8_t GetBus(); // Método para obter o barramento do aquecedor
  void TurnOff(); // Método para desligar o aquecedor
  bool InvertedPWM(); // Método para verificar se o PWM é invertido
  void SetPidParameters(double input, double setpoint); // Método para definir os parâmetros do PID
};
#endif
