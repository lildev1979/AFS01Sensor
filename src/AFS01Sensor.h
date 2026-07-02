#ifndef AFS01_SENSOR_H
#define AFS01_SENSOR_H

#include <Arduino.h>
#include <Wire.h>

class AFS01Sensor {
private:
    const uint8_t _address = 0x40; // Default I2C Address
    uint8_t calculateCRC8(uint8_t *data, uint8_t len);

public:
    AFS01Sensor(); // Constructor
    void begin();
    bool readFlow(int16_t &flowRate);
    bool readSensorID(uint32_t &sensorID);
};

#endif