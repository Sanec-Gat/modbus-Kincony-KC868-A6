#ifndef TROYKA_THERMOMETER_ESP32_H
#define TROYKA_THERMOMETER_ESP32_H

#include <Arduino.h>

class TroykaThermometerESP32 {
public:
    TroykaThermometerESP32(uint8_t pin);
    void begin();
    void read();
    float getTemperatureC();
    float getTemperatureK();
    float getTemperatureF();
    
private:
    uint8_t _pin;
    float _temperatureC;
    float _temperatureK;
    float _temperatureF;
};

#endif