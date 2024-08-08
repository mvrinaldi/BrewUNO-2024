//@(#) input_pcf.h

#ifndef KeyButton_H
#define KeyButton_H

#include <pcf8574_esp.h>

class KeyButton
{
private:
  const uint16_t pin;
  PCF857x &pcf;
  unsigned long timer = 0;
  unsigned long long_timer = 1500;
  bool state = false;
  bool state_long = false;

public:
  bool pressed_long = false;
  bool pressed = false;

  KeyButton(const uint16_t pin, PCF857x &pcf_ref) : pin(pin), pcf(pcf_ref)
  {
  }

  void Update()
  {
    pressed = false;
    pressed_long = false;

    //button pressed
    if (!pcf.read(pin) && !state && millis() - timer >= 50)
    {
      state = true;
      timer = millis();
    }
    //button pressed long
    else if (!pcf.read(pin) && state && !state_long && millis() - timer >= long_timer)
    {
      state_long = true;
      pressed_long = true;
    }
    //button pressed short
    else if (pcf.read(pin) && state && millis() - timer >= 50)
    {
      if (millis() - timer < long_timer)
      pressed = true;
      state_long = false;
      state = false;
      timer = millis();
    }
  }
};

#endif
