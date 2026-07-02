#include "AFS01Sensor.h"

// Constructor
AFS01Sensor::AFS01Sensor() {}

// Initialize the I2C communication at standard 100kHz speed[cite: 1]
void AFS01Sensor::begin() {
    Wire.begin();
    Wire.setClock(100000); // 100 kHz standard speed[cite: 1]
}

// Private helper to calculate the custom AFS01 CRC8 polynomial[cite: 1]
uint8_t AFS01Sensor::calculateCRC8(uint8_t *data, uint8_t len) {
    uint8_t crc = 0x00; // Initial value[cite: 1]
    for (uint8_t byte = 0; byte < len; byte++) {
        crc ^= data[byte];[cite: 1]
        for (uint8_t bit = 8; bit > 0; --bit) {[cite: 1]
            if (crc & 0x80) {[cite: 1]
                crc = (crc << 1) ^ 0x131; // Polynomial: 0x31[cite: 1]
            } else {
                crc = (crc << 1);[cite: 1]
            }
        }
    }
    return crc;[cite: 1]
}

// Requests the flow data, processes signed values, and validates integrity
bool AFS01Sensor::readFlow(int16_t &flowRate) {
    // 1. Send the flow collection trigger command (0x10, 0x00)[cite: 1]
    Wire.beginTransmission(_address);[cite: 1]
    Wire.write(0x10); // Byte 1[cite: 1]
    Wire.write(0x00); // Byte 2[cite: 1]
    if (Wire.endTransmission() != 0) {
        return false; // Bus error or device NACK
    }
    
    // 2. Request 3 bytes from the sensor (High Byte, Low Byte, CRC)[cite: 1]
    uint8_t bytesReceived = Wire.requestFrom(_address, (uint8_t)3);[cite: 1]
    if (bytesReceived != 3) {
        return false; 
    }

    uint8_t rawBuffer[2];
    rawBuffer[0] = Wire.read(); // Flow High 8 bits[cite: 1]
    rawBuffer[1] = Wire.read(); // Flow Low 8 bits[cite: 1]
    uint8_t expectedCRC = Wire.read(); // CRC[cite: 1]

    // 3. Verify data using the manufacturer's algorithm[cite: 1]
    if (calculateCRC8(rawBuffer, 2) != expectedCRC) {
        return false; // Checksum validation failed
    }

    // 4. Combine bytes into signed 16-bit int to preserve negative flow values[cite: 1]
    flowRate = (int16_t)((rawBuffer[0] << 8) | rawBuffer[1]);
    return true;
}

// Requests the unique 32-bit Sensor ID[cite: 1]
bool AFS01Sensor::readSensorID(uint32_t &sensorID) {
    Wire.beginTransmission(_address);[cite: 1]
    Wire.write(0x31); // Byte 1[cite: 1]
    Wire.write(0xAE); // Byte 2[cite: 1]
    if (Wire.endTransmission() != 0) {
        return false;
    }

    if (Wire.requestFrom(_address, (uint8_t)6) != 6) {[cite: 1]
        return false;
    }

    uint8_t idPart1[2], idPart2[2];
    idPart1[0] = Wire.read(); // ID High Byte [31:24][cite: 1]
    idPart1[1] = Wire.read(); // ID Mid Byte [23:16][cite: 1]
    uint8_t crc1 = Wire.read(); // CRC for first segment[cite: 1]

    idPart2[0] = Wire.read(); // ID Mid Byte [15:8][cite: 1]
    idPart2[1] = Wire.read(); // ID Low Byte [7:0][cite: 1]
    uint8_t crc2 = Wire.read(); // CRC for second segment[cite: 1]

    // Validate segmented arrays[cite: 1]
    if (calculateCRC8(idPart1, 2) != crc1 || calculateCRC8(idPart2, 2) != crc2) {
        return false; 
    }

    // Construct final 32-bit container
    sensorID = ((uint32_t)idPart1[0] << 24) | 
               ((uint32_t)idPart1[1] << 16) | 
               ((uint32_t)idPart2[0] << 8)  | 
               (uint32_t)idPart2[1];
    return true;
}