#pragma once

#include <stdint.h>

#include "GpioLogic.h"

void gpio_ext_set(uint8_t address, uint8_t pin, bool value);
uint8_t gpio_ext_get(uint8_t address, uint8_t pin);

class GpioExtPin : public GpioPin
{
  public:
    GpioExtPin(uint16_t _pin) : pin(_pin & 0x3F), address(_pin & 0x100 ? 0x44 : 0x43){};
    uint8_t pin;
    uint8_t address;

    void set(bool value) override { gpio_ext_set(address, pin, value); }
    uint8_t get() { return gpio_ext_get(address, pin); }
};
