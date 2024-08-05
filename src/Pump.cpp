#include "Pump.h"

// Variáveis para armazenar os tempos usando millis()
unsigned long lastPumpStarted = 0;
unsigned long lastPumpRest = 0;

bool isResting = false;
bool recirculationOn = false;

Pump::Pump(AsyncWebServer *server, ActiveStatus *activeStatus, BrewSettingsService *brewSettingsService) 
    : _server(server), _activeStatus(activeStatus), _brewSettingsService(brewSettingsService)
{
    _server->on(START_PUMP_SERVICE_PATH, HTTP_POST, std::bind(&Pump::startPumpHttpService, this, std::placeholders::_1));
    _server->on(STOP_PUMP_SERVICE_PATH, HTTP_POST, std::bind(&Pump::stopPumpHttpService, this, std::placeholders::_1));
}

void Pump::startPumpHttpService(AsyncWebServerRequest *request)
{
    TurnPumpOn();
    request->send(200, APPLICATION_JSON_TYPE, _activeStatus->GetJson());
}

void Pump::stopPumpHttpService(AsyncWebServerRequest *request)
{
    TurnPumpOff();
    request->send(200, APPLICATION_JSON_TYPE, _activeStatus->GetJson());
}

void Pump::TurnPumpOn()
{
    TurnPump(true);
    recirculationOn = true;
    lastPumpStarted = millis();
    lastPumpRest = 0;
    CheckRest();
}

void Pump::TurnPumpOff()
{
    TurnPump(false);
    recirculationOn = false;
    lastPumpStarted = 0;
    lastPumpRest = 0;
    CheckRest();
}

void Pump::TurnPump(bool on)
{
    on = _brewSettingsService->InvertPump ? !on : on;
    digitalWrite(PUMP_BUS, on ? HIGH : LOW);
    _activeStatus->PumpOn = _brewSettingsService->InvertPump ? !on : on;
}

void Pump::CheckRest()
{
    if (recirculationOn && !_activeStatus->PIDActing)
    {
        unsigned long timeNow = millis();
        if (!isResting && timeNow - lastPumpStarted >= _brewSettingsService->PumpRestInterval * 1000)
        {
            Serial.println("Pump Rest time to sleep!");
            TurnPump(false);
            isResting = true;
            lastPumpStarted = 0;
            lastPumpRest = timeNow;
            Buzzer().Ring(1, 50);
        }

        if (isResting && timeNow - lastPumpRest >= _brewSettingsService->PumpRestTime * 1000)
        {
            Serial.println("Pump Rest wake up!");
            TurnPump(true);
            isResting = false;
            lastPumpRest = 0;
            lastPumpStarted = timeNow;
            Buzzer().Ring(1, 50);
        }
        _activeStatus->PumpIsResting = isResting;
    }
    else
    {
        _activeStatus->PumpIsResting = false;
    }
}

void Pump::antiCavitation()
{
    if (_activeStatus->ActiveStep == anticavitation)
    {
        for (byte i = 1; i < 6; i++)
        {
            TurnPump(true);
            delay(1500 + i * 250);
            TurnPump(false);
            delay(2000);
        }
        _activeStatus->ActiveStep = _activeStatus->LastActiveStep;
    }
}
