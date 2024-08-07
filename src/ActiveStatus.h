#ifndef ActiveStatus_h
#define ActiveStatus_h

#if defined(ESP8266)
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#elif defined(ESP_PLATFORM)
#include <WiFi.h>
#include <AsyncTCP.h>
#endif

#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>
#include <AsyncArduinoJson6.h>
#include <TimeLib.h>
#include <NtpClientLib.h>
#include "enum.h"

#define PGM_P       const char *
#define PGM_VOID_P  const void *
#define PSTR(s) (__extension__({static const char __c[] PROGMEM = (s); &__c[0];}))

#define MAX_ACTIVESTATUS_SIZE 2024
#define ACTIVE_STATUS_FILE "/config/activeStatus.json"

class ActiveStatus {
public:
  ActiveStatus(FS *fs);
  ~ActiveStatus();

  // Step Information
  int ActiveStep;
  int LastActiveStep;
  bool BrewStarted;

  // Boil Step Information
  String ActiveBoilStepIndex;
  String ActiveBoilStepName;

  int ActiveMashStepIndex;
  String ActiveMashStepName;
  String ActiveMashStepSufixName;

  int BoilTime;
  double PWM;
  double PWMPercentage;
  double SpargePWM;
  double SpargePWMPercentage;
  double BoilPWM;
  double BoilPWMPercentage;
  bool PIDActing;
  bool StartBoilCounter;

  double BoilTargetTemperature;
  double BoilPowerPercentage;

  double TargetTemperature;
  double Temperature;
  double BoilTemperature;
  double SpargeTemperature;
  double AuxOneTemperature;
  double AuxTwoTemperature;
  double AuxThreeTemperature;

  bool EnableSparge;
  bool EnableBoilKettle;
  double SpargeTargetTemperature;

  String MainSensor;
  String SpargeSensor;
  String BoilSensor;
  String AuxOneSensor;
  String AuxTwoSensor;
  String AuxThreeSensor;

  String Count;

  time_t EndTime;
  time_t StartTime;
  time_t TimeNow;

  bool Recirculation;
  bool PIDTuning;
  bool PumpOn;
  bool PumpIsResting;
  bool PIDSettingsUpdated;
  bool TimeNotSet;

  String TempUnit;

  bool HeaterOn;
  bool FullPower;
  bool StepLock;
  bool StepLocked;

  bool LoadActiveStatusSettings();
  void SaveActiveStatus(time_t StartTime,
                        time_t EndTime,
                        time_t TimeNow,
                        float TargetTemperature,
                        int ActiveMashStepIndex,
                        String ActiveBoilStepIndex,
                        int boilTime,
                        float boilTargetTemperature,
                        int ActiveStep,
                        bool brewStarted);
  void SaveActiveStatus();
  void SaveActiveStatusLoop();
  void SetTemperature(Temperatures temps);
  void SetJsonTemperatures(String json);
  String GetJson();
  void TimeNotSetted();
  void TimeSetted();

private:
  FS *_fs;
};
#endif
