#include "TroykaThermometerESP32.h"

TroykaThermometerESP32::TroykaThermometerESP32(uint8_t pin) {
    _pin = pin;
}

void TroykaThermometerESP32::begin() {
    // Настройка АЦП ESP32
    analogReadResolution(12);      // 12 бит (0-4095)
    analogSetAttenuation(ADC_11db); // Диапазон 0-3.3В
    pinMode(_pin, INPUT);
}

void TroykaThermometerESP32::read() {
    // Усреднение 32 измерений
    int sensorADC = 0;
    for (int i = 0; i < 32; i++) {
        sensorADC += analogRead(_pin);
        delayMicroseconds(100);
    }
    sensorADC = sensorADC / 32;
    
    // Конвертация в напряжение
    float voltage = sensorADC * (3.3 / 4095.0);
    
    // Расчет температуры для TMP36
    _temperatureC = (voltage - 0.5) * 100.0;
    _temperatureK = _temperatureC + 273.15;
    _temperatureF = (_temperatureC * 9.0 / 5.0) + 32.0;
}

float TroykaThermometerESP32::getTemperatureC() {
    return _temperatureC;
}

float TroykaThermometerESP32::getTemperatureK() {
    return _temperatureK;
}

float TroykaThermometerESP32::getTemperatureF() {
    return _temperatureF;
}