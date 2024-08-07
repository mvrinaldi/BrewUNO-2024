#include <BoilSettingsService.h>

BoilSettingsService::BoilSettingsService(AsyncWebServer *server, FS *fs, BrewSettingsService *brewSettings)
    : BrewListService(server, fs,
                      GET_BOIL_SETTINGS_SERVICE_PATH,
                      POST_BOIL_SETTINGS_SERVICE_PATH,
                      BOIL_SETTINGS_FILE),
      _brewSettings(brewSettings) {}



bool BoilSettingsService::jsonSchemaIsValid(JsonDocument &jsonObj, String &messages)
{
    JsonArray steps = jsonObj["st"];
    bool validJson = true;

    for (std::size_t i = 0; i < steps.size(); i++)
    {
        JsonObject step = steps.getElement(i);
        if (step["n"] == "")
        {
            validJson = false;
            messages += "Name could not be null. ";
        }
    }
    return validJson;
}