#include "BoilService.h"

// Construtor para inicializar o serviço de fervura
BoilService::BoilService(FS *fs, TemperatureService *temperatureService, BrewSettingsService *brewSettingsService) : _fs(fs),
                                                                                                                     _temperatureService(temperatureService),
                                                                                                                     _brewSettingsService(brewSettingsService)
{
}

// Destrutor
BoilService::~BoilService() {}

// Documento JSON para armazenar as configurações de fervura
DynamicJsonDocument jsonDocumentBoil = DynamicJsonDocument(MAX_ACTIVESTATUS_SIZE);
JsonObject _boilSettings;

// Carregar as configurações de fervura a partir do arquivo
void BoilService::LoadBoilSettings()
{
    File configFile = _fs->open(BOIL_SETTINGS_FILE, "r");
    if (!configFile) {
        Serial.println("Erro ao abrir o arquivo de configurações de fervura");
        return;
    }

    if (configFile.size() > MAX_ACTIVESTATUS_SIZE) {
        Serial.println("Arquivo de configurações de fervura muito grande");
        configFile.close();
        return;
    }

    DeserializationError error = deserializeJson(jsonDocumentBoil, configFile);
    if (error) {
        Serial.print("Erro ao desserializar o JSON: ");
        Serial.println(error.c_str());
        configFile.close();
        return;
    }

    if (!jsonDocumentBoil.is<JsonObject>()) {
        Serial.println("O JSON desserializado não é um objeto");
        configFile.close();
        return;
    }

    _boilSettings = jsonDocumentBoil.as<JsonObject>();
    configFile.close();
}

// Método principal de loop para o serviço de fervura
void BoilService::loop(ActiveStatus *activeStatus)
{
    if (!activeStatus->BrewStarted || activeStatus->ActiveStep != boil) {
        return;
    }

    time_t timeNow = now();
    activeStatus->BoilTargetTemperature = _brewSettingsService->BoilTemperature;

    if (activeStatus->StartTime == 0) {
        activeStatus->ActiveBoilStepName = "Heating to Boil";
    }

    if ((activeStatus->StartTime == 0 && activeStatus->BoilTemperature >= activeStatus->BoilTargetTemperature) || activeStatus->StartBoilCounter) {
        activeStatus->StartBoilCounter = false;
        activeStatus->StartTime = timeNow;
        activeStatus->EndTime = activeStatus->StartTime + activeStatus->BoilTime;
        activeStatus->ActiveBoilStepName = "";
        Serial.println("Boil started");
        Serial.println(activeStatus->StartTime);
        Serial.println(activeStatus->EndTime);
        Buzzer().Ring(1, 2000);
        activeStatus->SaveActiveStatus();
    }

    if (activeStatus->EndTime > 0 && timeNow > activeStatus->EndTime) {
        Serial.println("Boil ended");
        Buzzer().Ring(1, 2000);
        activeStatus->SaveActiveStatus(0, 0, 0, 0, -1, "", 0, 0, none, false);
        return;
    }

    SetBoiIndexStep(activeStatus, activeStatus->EndTime - timeNow);
}

// Define o índice do passo de fervura com base no tempo restante
void BoilService::SetBoiIndexStep(ActiveStatus *activeStatus, int second)
{
    int index = 0;
    String currentStep = "";
    String currentStepName = "";
    JsonArray steps = _boilSettings["st"].as<JsonArray>();

    for (auto step : steps) {
        int time = step["tm"];
        if (time * 60 == second) {
            String name = step["n"];
            String time = step["tm"];
            String amount = step["a"];
            currentStep = currentStep.isEmpty() ? String(index) : currentStep + "," + String(index);
            currentStepName = currentStepName.isEmpty() ? getStepName(name, time, amount) : currentStepName + "/" + getStepName(name, time, amount);
        }
        index++;
    }

    if (!currentStep.isEmpty() && currentStep != activeStatus->ActiveBoilStepIndex) {
        activeStatus->ActiveBoilStepIndex = currentStep;
        activeStatus->ActiveBoilStepName = currentStepName;
        Serial.println(currentStep);
        Serial.println(activeStatus->ActiveBoilStepIndex);
        Buzzer().Ring(3);
        Buzzer().Ring(1, 1000);
        activeStatus->SaveActiveStatus();
    }
}

// Retorna o nome do passo com base nos parâmetros fornecidos
String BoilService::getStepName(String name, String time, String amount)
{
    return name + " " + amount + "g@" + time + "'";
}